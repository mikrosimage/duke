#include "AttributeDisplay.hpp"
#include <duke/base/Slice.hpp>

void AttributeDisplay::snprintf(const Attribute& attribute,
                                StringAppender& appender) const {
  const char* const pTypeName = attribute.key.type_index.name();
  const auto pFound = m_TypeToDisplayFunctions.find(pTypeName);
  if (pFound == m_TypeToDisplayFunctions.end()) {
    const uint8_t* const begin = attribute.value.begin();
    const uint8_t* const end = attribute.value.end();
    for (const uint8_t* pSrc = begin; !appender.done() && pSrc != end; ++pSrc)
      appender.snprintf("%s%02X", pSrc != begin ? "-" : "", *pSrc);
    return;
  }
  pFound->second(attribute.value.begin(), attribute.value.size(), appender);
}

template <>
void displayValue<const char*>(const void* ptr, size_t byteSize,
                               StringAppender& appender) {
  appender.snprintf(R"("%s")", *reinterpret_cast<const char* const*>(ptr));
}
template <>
void displayValue<char>(const void* ptr, size_t byteSize,
                        StringAppender& appender) {
  appender.snprintf("'%c'", *reinterpret_cast<const char*>(ptr));
}
template <>
void displayValue<unsigned char>(const void* ptr, size_t byteSize,
                                 StringAppender& appender) {
  appender.snprintf("%d", *reinterpret_cast<const unsigned char*>(ptr));
}
template <>
void displayValue<int>(const void* ptr, size_t byteSize,
                       StringAppender& appender) {
  appender.snprintf("%d", *reinterpret_cast<const int*>(ptr));
}
template <>
void displayValue<long>(const void* ptr, size_t byteSize,
                        StringAppender& appender) {
  appender.snprintf("%ld", *reinterpret_cast<const long*>(ptr));
}
template <>
void displayValue<long long>(const void* ptr, size_t byteSize,
                             StringAppender& appender) {
  appender.snprintf("%lld", *reinterpret_cast<const long long*>(ptr));
}
template <>
void displayValue<unsigned>(const void* ptr, size_t byteSize,
                            StringAppender& appender) {
  appender.snprintf("%u", *reinterpret_cast<const unsigned*>(ptr));
}
template <>
void displayValue<unsigned long>(const void* ptr, size_t byteSize,
                                 StringAppender& appender) {
  appender.snprintf("%lu", *reinterpret_cast<const unsigned long*>(ptr));
}
template <>
void displayValue<unsigned long long>(const void* ptr, size_t byteSize,
                                      StringAppender& appender) {
  appender.snprintf("%llu", *reinterpret_cast<const unsigned long long*>(ptr));
}
template <>
void displayValue<float>(const void* ptr, size_t byteSize,
                         StringAppender& appender) {
  appender.snprintf("%f", *reinterpret_cast<const float*>(ptr));
}
template <>
void displayValue<double>(const void* ptr, size_t byteSize,
                          StringAppender& appender) {
  appender.snprintf("%f", *reinterpret_cast<const double*>(ptr));
}
template <>
void displayValue<long double>(const void* ptr, size_t byteSize,
                               StringAppender& appender) {
  appender.snprintf("%Lf", *reinterpret_cast<const long double*>(ptr));
}

template <>
void displayArray<char>(const void* ptr, size_t byteSize, StringAppender& appender) {
	appender.snprintf(R"("%s")", reinterpret_cast<const char* >(ptr));
}

template <typename T>
void AttributeDisplay::registerType() {
  m_TypeToDisplayFunctions[typeid(T).name()] = &displayValue<T>;
  m_TypeToDisplayFunctions[typeid(Slice<T>).name()] = &displayArray<T>;
}

void registerPrimitiveTypes(AttributeDisplay& display) {
  display.registerType<const char*>();
  display.registerType<char>();
  display.registerType<int>();
  display.registerType<long>();
  display.registerType<long long>();
  display.registerType<unsigned char>();
  display.registerType<unsigned>();
  display.registerType<unsigned long>();
  display.registerType<unsigned long long>();
  display.registerType<float>();
  display.registerType<double>();
  display.registerType<long double>();
}
