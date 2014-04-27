#include "StringUtils.hpp"
#include <duke/base/Check.hpp>

bool streq(const char* first, const char* second) {
  if (first == second) return first;  // same pointer or both nullptr
  for (;; ++first, ++second) {
    if (*first != *second) return false;
    if (*first == '\0') return true;
  }
  return false;
}

bool strless(const char* first, const char* second) {
  if (first == second) return false;  // same pointer or both nullptr
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

void appendPaddedFrameNumber(const size_t frame, unsigned char padding, std::string& output) {
  enum {
    kMaxDigits = 10
  };
  CHECK(padding <= kMaxDigits);
  char buffer[padding];
  char* pChar = buffer;
  for (size_t remainder = frame; padding; ++pChar, --padding, remainder /= 10) *pChar = '0' + remainder % 10;
  for (--pChar; pChar >= buffer; --pChar) output.push_back(*pChar);
}

bool nocase_compare::operator()(const unsigned char& c1, const unsigned char& c2) const {
  return tolower(c1) < tolower(c2);
}

bool ci_less::operator()(const std::string& s1, const std::string& s2) const {
  return std::lexicographical_compare(s1.begin(), s1.end(),  // source range
                                      s2.begin(), s2.end(),  // dest range
                                      nocase_compare());     // comparison
}
