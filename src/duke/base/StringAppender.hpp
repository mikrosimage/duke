#pragma once

#include <duke/base/Check.hpp>
#include <duke/base/NonCopyable.hpp>
#include <duke/base/Slice.hpp>

#include <cstring>

/**
 * A small struct to append strings to a buffer.
 * If you try to write too many more characters, the output is truncated.
 */
struct StringAppender : public noncopyable {
  StringAppender(char* const ptr, size_t size) : m_Slice(ptr, ptr + size) {
    CHECK(!m_Slice.empty()) << "buffer should be at least 1 char to output '\0'";
    m_Slice = m_Slice.pop_back(1);  // reserving one last char for
    m_Slice.front() = '\0';
  }

  template <int size>
  StringAppender(char (&array)[size])
      : StringAppender(array, size) {}

  void append(const Slice<const char> input) {
    if (m_Slice.empty()) return;
    const Slice<const char> resized = input.resize(m_Slice.size());
    std::copy(resized.begin(), resized.end(), m_Slice.begin());
    m_Slice = m_Slice.pop_front(resized.size());
    // We reserved a char in the constructor so even if slice is empty
    // we can write to front which, in this case, will be the last char
    // of the buffer.
    m_Slice.front() = '\0';
  }

  void append(const char input) {
    if (m_Slice.empty()) return;
    m_Slice.front() = input;
    m_Slice = m_Slice.pop_front(1);
    // We reserved a char in the constructor so even if slice is empty
    // we can write to front which, in this case, will be the last char
    // of the buffer.
    m_Slice.front() = '\0';
  }

  inline void append(const char* array) { append(Slice<const char>(array, array + strlen(array))); }

  inline void append(const std::string& string) {
    append(Slice<const char>(string.data(), string.data() + string.size()));
  }

  bool full() const { return m_Slice.empty(); }

  operator bool() const { return !full(); }

 private:
  Slice<char> m_Slice;
};

template <size_t size>
struct BufferStringAppender : public StringAppender {
  BufferStringAppender() : StringAppender(m_Buffer) {}

  const char* c_str() const { return m_Buffer; }

 private:
  char m_Buffer[size];
};
