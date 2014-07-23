#include "duke/base/StringAppender.hpp"

#include <algorithm>

namespace {

const char kBase[] = "0123456789ABCDEF";

}  // namespace

StringAppender::StringAppender(char* const ptr, size_t size) : m_BufferSlice(ptr, ptr + size), m_TotalSize(size - 1) {
  CHECK(!m_BufferSlice.empty()) << "buffer should be at least 1 char to output '\0'";
  m_BufferSlice = pop_back(m_BufferSlice, 1);  // reserving one last char for
  m_BufferSlice.front() = '\0';
}

void StringAppender::append(const StringSlice input) {
  if (m_BufferSlice.empty()) return;
  const auto resized = keep_front(input, m_BufferSlice.size());
  std::copy(resized.begin(), resized.end(), m_BufferSlice.begin());
  m_BufferSlice = pop_front(m_BufferSlice, resized.size());
  // We reserved a char in the constructor so even if slice is empty
  // we can write to front which, in this case, will be the last char
  // of the buffer.
  m_BufferSlice.front() = '\0';
}

void StringAppender::append(const char input) {
  if (m_BufferSlice.empty()) return;
  m_BufferSlice.front() = input;
  m_BufferSlice = pop_front(m_BufferSlice, 1);
  // We reserved a char in the constructor so even if slice is empty
  // we can write to front which, in this case, will be the last char
  // of the buffer.
  m_BufferSlice.front() = '\0';
}

void StringAppender::appendInteger(uint64_t value, uint8_t base) {
  CHECK(base <= strlen(kBase));
  auto pBegin = m_BufferSlice.begin();
  size_t digits = 0;
  while (value && !full()) {
    append(kBase[value % base]);
    value /= base;
    ++digits;
  }
  std::reverse(pBegin, pBegin + digits);
}
