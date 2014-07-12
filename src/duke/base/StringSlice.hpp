#pragma once

#include "duke/base/Slice.hpp"

#include <iostream>

#include <cstring>

struct StringSlice : public Slice<const char> {
  typedef Slice<const char> UP;
  using UP::UP;
  StringSlice() : UP() {}
  StringSlice(const char* str, const size_t count) : UP(str, str + count) {}
  StringSlice(const char* str) : StringSlice(str, strlen(str)) {}
  StringSlice(const std::string& str) : UP(&*str.begin(), &*str.end()) {}
  template <int size>
  StringSlice(char (&array)[size])
      : StringSlice(array, size) {}

  bool operator==(StringSlice other) const { return UP::operator==(other); }
  bool operator<(StringSlice other) const { return UP::operator<(other); }

  bool startsWith(StringSlice prefix) const {
    if (prefix.size() > size()) return false;
    return keep_front(*this, prefix.size()) == prefix;
  }

  bool endsWith(StringSlice prefix) const {
    if (prefix.size() > size()) return false;
    return keep_back(*this, prefix.size()) == prefix;
  }

  std::string ToString() const { return std::string(begin(), size()); }
};

std::ostream& operator<<(std::ostream& stream, const StringSlice& slice);
