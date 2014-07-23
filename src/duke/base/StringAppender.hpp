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
  StringAppender(char* const ptr, size_t size);

  template <int size>
  StringAppender(char (&array)[size])
      : StringAppender(array, size) {}

  void append(const StringSlice input);

  void append(const char input);

  void appendInteger(uint64_t value, uint8_t base = 10);

  bool full() const { return m_BufferSlice.empty(); }

  size_t size() const { return m_TotalSize - m_BufferSlice.size(); }

  bool empty() const { return size() == 0; }

  operator bool() const { return !full(); }

 private:
  Slice<char> m_BufferSlice;
  const size_t m_TotalSize;
};

template <size_t SIZE>
struct BufferStringAppender : public StringAppender {
  BufferStringAppender() : StringAppender(m_Buffer) {}

  const char* c_str() const { return m_Buffer; }

  StringSlice slice() const { return StringSlice(m_Buffer, size()); }

 private:
  char m_Buffer[SIZE];
};
