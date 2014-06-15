#pragma once

#include "duke/base/Check.hpp"
#include "duke/base/NonCopyable.hpp"
#include "duke/base/StringSlice.hpp"

#include <cstring>

/**
 * A small struct to append strings to a buffer.
 * If you try to write too many more characters, the output is truncated.
 */
struct StringAppender : public noncopyable {
  StringAppender(char* const ptr, size_t size) : m_BufferSlice(ptr, ptr + size) {
    CHECK(!m_BufferSlice.empty()) << "buffer should be at least 1 char to output '\0'";
    m_BufferSlice = pop_back(m_BufferSlice, 1);  // reserving one last char for
    m_BufferSlice.front() = '\0';
  }

  template <int size>
  StringAppender(char (&array)[size])
      : StringAppender(array, size) {}

  void append(const StringSlice input) {
    if (m_BufferSlice.empty()) return;
    const auto resized = keep_front(input, m_BufferSlice.size());
    std::copy(resized.begin(), resized.end(), m_BufferSlice.begin());
    m_BufferSlice = pop_front(m_BufferSlice, resized.size());
    // We reserved a char in the constructor so even if slice is empty
    // we can write to front which, in this case, will be the last char
    // of the buffer.
    m_BufferSlice.front() = '\0';
  }

  void append(const char input) {
    if (m_BufferSlice.empty()) return;
    m_BufferSlice.front() = input;
    m_BufferSlice = pop_front(m_BufferSlice, 1);
    // We reserved a char in the constructor so even if slice is empty
    // we can write to front which, in this case, will be the last char
    // of the buffer.
    m_BufferSlice.front() = '\0';
  }

  bool full() const { return m_BufferSlice.empty(); }

  operator bool() const { return !full(); }

 private:
  Slice<char> m_BufferSlice;
};

template <size_t size>
struct BufferStringAppender : public StringAppender {
  BufferStringAppender() : StringAppender(m_Buffer) {}

  const char* c_str() const { return m_Buffer; }

 private:
  char m_Buffer[size];
};
