#include "StringUtils.hpp"
#include <duke/base/Check.hpp>
#include <duke/base/StringAppender.hpp>

#include <limits>

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
