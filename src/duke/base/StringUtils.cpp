#include "StringUtils.hpp"
#include "duke/base/Check.hpp"
#include "duke/base/StringAppender.hpp"

#include <limits>
#include <algorithm>

bool streq(const char* first, const char* second) {
  CHECK(first && second);
  if (first == second) return true;  // same pointer
  for (;; ++first, ++second) {
    if (*first != *second) return false;
    if (*first == '\0') return true;
  }
  return false;
}

bool strless(const char* first, const char* second) {
  CHECK(first && second);
  if (first == second) return false;  // same pointer
  for (;; ++first, ++second) {
    if (*first != *second) return *first < *second;
    if (*first == '\0') return true;
  }
  return false;
}

void strdim(const char* string, unsigned& width, unsigned& height) {
  CHECK(string);
  width = 0;
  height = 0;
  const char* start = string;
  const char* end = strchr(start, '\n');
  while (end != nullptr) {
    ++height;
    CHECK(end - start < std::numeric_limits<unsigned>::max());
    width = std::max(width, static_cast<unsigned>(end - start));
    start = end + 1;
    end = strchr(start, '\n');
  }
  width = std::max(width, static_cast<unsigned>(strlen(start)));
}

unsigned char digits(size_t frame) {
  unsigned char count = 1;
  for (; frame > 9; frame /= 10, ++count)
    ;
  return count;
}

void appendPaddedFrameNumber(const size_t frame, unsigned char padding, StringAppender& output) {
  CHECK(padding <= std::numeric_limits<decltype(frame)>::digits10);
  char buffer[padding];
  char* pChar = buffer;
  for (size_t remainder = frame; padding; ++pChar, --padding, remainder /= 10) *pChar = '0' + remainder % 10;
  for (--pChar; pChar >= buffer; --pChar) output.append(*pChar);
}

bool nocase_compare::operator()(const unsigned char& c1, const unsigned char& c2) const {
  return tolower(c1) < tolower(c2);
}

bool ci_less::operator()(const std::string& s1, const std::string& s2) const {
  return std::lexicographical_compare(s1.begin(), s1.end(),  // source range
                                      s2.begin(), s2.end(),  // dest range
                                      nocase_compare());     // comparison
}

bool stripPrefix(StringSlice prefix, StringSlice& from) {
  if (!from.startsWith(prefix)) return false;
  from = pop_front(from, prefix.size());
  return true;
}

bool stripSuffix(StringSlice suffix, StringSlice& from) {
  if (!from.endsWith(suffix)) return false;
  from = pop_back(from, suffix.size());
  return true;
}
