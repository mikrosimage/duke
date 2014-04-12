#pragma once
#pragma GCC diagnostic ignored "-Wformat-security"

#include <duke/attributes/Attribute.hpp>
#include <duke/base/Check.hpp>
#include <map>

/**
 * A small struct to append formatted data to a buffer.
 * If you try to write too many more characters, the output is truncated and
 * "..." is appended at the end.
 */
struct StringAppender {
  StringAppender(char* const ptr, size_t size)
      : m_pCurrent(ptr), m_Remaining(size), m_Size(size) {
	  CHECK(m_Size>0);
	  m_pCurrent[0] = '\0';
  }

  template <typename... Args>
  inline void snprintf(const char* string, Args... args) {
    if (m_Remaining == 0) return;
    const auto written = ::snprintf(m_pCurrent, m_Remaining, string, args...);
    if (written >= m_Remaining) {
      m_pCurrent += m_Remaining;
      m_Remaining = 0;
      // Finishing string by an ellipsis '...'
      const auto dotCount = std::min(m_Size, 4LU) - 1;
      memset(m_pCurrent - dotCount - 1, '.', dotCount);
    } else {
      m_Remaining -= written;
      m_pCurrent += written;
    }
  }

  bool done() const { return m_Remaining == 0; }

private:
  char* m_pCurrent;
  size_t m_Remaining;
  const size_t m_Size;
};

/**
 * An object to display an attribute depending on its runtime type.
 */
class AttributeDisplay {
 public:
  // Registers a function to display T.
  template <typename T>
  void registerType();

  // Printf from a StringAppender directly
  void snprintf(const Attribute& attribute, StringAppender& appender) const;

  // Printf from a raw buffer
  inline void snprintf(const Attribute& attribute, char* const string,
                       const size_t n) const {
	StringAppender appender{string, n};
    snprintf(attribute, appender);
  }

 private:
  typedef std::function<void(const void*, size_t, StringAppender&)> DisplayFunc;
  std::map<const char*, DisplayFunc> m_TypeToDisplayFunctions;
};

// Make sure all the primitive types are registered for display.
// Don't forget to call display.registerType<>() with you own custom types.
void registerPrimitiveTypes(AttributeDisplay& display);

/**
 * Generic method to display a value of type T.
 *
 * Default implementation does not compile so make sure you provide an
 *implementation for your own type.
 */
template <typename T>
void displayValue(const void*, size_t, StringAppender&) {
  static_assert(true, "No specialization available");
}

/**
 * Automatically implements a function to display an array of T by using
 * displayValue<T>.
 */
template <typename T>
void displayArray(const void* ptr, size_t byteSize, StringAppender& appender) {
  if (byteSize % sizeof(T) != 0) throw std::runtime_error("bad size");
  const T* pBegin = reinterpret_cast<const T*>(ptr);
  const T* const pEnd = reinterpret_cast<const T*>(ptr) + byteSize / sizeof(T);
  appender.snprintf("[");
  for (auto itr = pBegin; itr != pEnd; ++itr) {
    if (itr != pBegin) appender.snprintf(",");
    displayValue<T>(itr, sizeof(T), appender);
  }
  appender.snprintf("]");
}
