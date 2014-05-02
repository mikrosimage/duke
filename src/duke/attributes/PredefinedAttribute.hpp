#pragma once

#include "duke/attributes/Attributes.hpp"
#include "duke/attributes/AttributeTraits.hpp"

namespace attribute {

#define DECLARE_ATTRIBUTE(NAME, TYPE, KEY, DEFAULT_VALUE)                           \
  struct NAME {                                                                     \
    constexpr static const char* key = KEY;                                         \
    typedef TYPE static_type;                                                       \
    constexpr static static_type default_value = DEFAULT_VALUE;                     \
    constexpr static attribute::Type type = attribute::runtime_type<static_type>(); \
    constexpr static bool is_array = attribute::is_array<type>();                   \
    typedef attribute::return_type<type>::value return_type;                        \
  }

#define DECLARE_ARRAY_ATTRIBUTE(NAME, TYPE, KEY)                                    \
  struct NAME {                                                                     \
    constexpr static const char* key = KEY;                                         \
    typedef Slice<const TYPE> static_type;                                          \
    constexpr static attribute::Type type = attribute::runtime_type<static_type>(); \
    constexpr static bool is_array = attribute::is_array<type>();                   \
    typedef attribute::return_type<type>::value return_type;                        \
  }

namespace details {

inline void checkRuntimeType(const Type actual, const Type expected) {
  CHECK(actual == expected) << "Type expected to be " << getTypeString(expected) << " but was "
                            << getTypeString(actual);
}

template <bool IS_ARRAY>
struct Getter {
  template <typename Prototype>
  inline static typename Prototype::return_type typify(const MemorySlice& memorySlice);
};

template <>
template <typename Prototype>
inline typename Prototype::return_type Getter<true>::typify(const MemorySlice& memorySlice) {
  typedef typename Prototype::static_type SliceType;
  typedef typename SliceType::value_type value_type;
  return asArray<value_type>(memorySlice);
}

template <>
template <typename Prototype>
inline typename Prototype::return_type Getter<false>::typify(const MemorySlice& memorySlice) {
  return asValue<typename Prototype::static_type>(memorySlice);
}

}  // namespace details
template <typename Prototype>
inline bool contains(const Attributes& attributes) {
  const auto pFound = find(attributes, Prototype::key);
  if (pFound == attributes.end()) return false;
  details::checkRuntimeType(pFound->type, Prototype::type);
  return true;
}

template <typename Prototype>
inline typename Prototype::return_type getWithDefault(const Attributes& attributes,
                                                      typename Prototype::return_type defaultValue) {
  const Attribute& attribute = get(attributes, Prototype::key);
  if (attribute.type == Type::Invalid || attribute.name == nullptr) return defaultValue;
  details::checkRuntimeType(attribute.type, Prototype::type);
  const MemorySlice data(attribute.value.begin(), attribute.value.end());
  return details::Getter<Prototype::is_array>::template typify<Prototype>(data);
}

template <typename Prototype>
inline typename Prototype::return_type getWithDefault(const Attributes& attributes) {
  return getWithDefault<Prototype>(attributes, Prototype::default_value);
}

template <typename Prototype>
inline typename Prototype::return_type getOrDie(const Attributes& attributes) {
  const Attribute& attribute = getOrDie(attributes, Prototype::key);
  details::checkRuntimeType(attribute.type, Prototype::type);
  const MemorySlice data(attribute.value.begin(), attribute.value.end());
  return details::Getter<Prototype::is_array>::template typify<Prototype>(data);
}

template <typename Prototype, typename = typename std::enable_if<Prototype::is_array == false>::type>
inline void set(Attributes& attributes, typename Prototype::static_type data) {
  set(attributes, Prototype::key, data);
}

template <typename Prototype, typename = typename std::enable_if<Prototype::is_array>::type, typename container>
inline void set(Attributes& attributes, container&& data) {
  typedef typename container::value_type actual_type;
  typedef typename Prototype::static_type::value_type expected_type;
  static_assert(runtime_type<actual_type>() == runtime_type<expected_type>(), "Incompatible type ");
  set(attributes, Prototype::key, data);
}

template <typename Prototype, typename = typename std::enable_if<Prototype::is_array == false>::type>
inline bool setIfNotPresent(Attributes& attributes, typename Prototype::static_type data) {
  return setIfNotPresent(attributes, Prototype::key, data);
}

template <typename Prototype, typename = typename std::enable_if<Prototype::is_array>::type, typename container>
inline bool setIfNotPresent(Attributes& attributes, container&& data) {
  typedef typename container::value_type actual_type;
  typedef typename Prototype::static_type::value_type expected_type;
  static_assert(runtime_type<actual_type>() == runtime_type<expected_type>(), "Incompatible type ");
  return setIfNotPresent(attributes, Prototype::key, std::forward<container>(data));
}

template <typename Prototype>
inline void erase(Attributes& attributes) {
  erase(attributes, Prototype::key);
}

}  // namespace attribute
