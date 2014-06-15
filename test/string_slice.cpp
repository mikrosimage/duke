#include "duke/base/StringSlice.hpp"

#include <gtest/gtest.h>

TEST(StringSlice, Equals) {
  const char const_char_array[] = "hello";
  const char *const_char = "hello";
  const std::string string = "hello";
  EXPECT_EQ(StringSlice(const_char_array), StringSlice(const_char));
  EXPECT_EQ(StringSlice(const_char_array), StringSlice(string));
  EXPECT_EQ(StringSlice(string), StringSlice(const_char));
}

TEST(StringSlice, prefix) {
  const StringSlice string = "abcdef";
  EXPECT_TRUE(string.startsWith("a"));
  EXPECT_TRUE(string.startsWith("abcdef"));
  EXPECT_FALSE(string.startsWith("z"));
}

TEST(StringSlice, suffix) {
  const StringSlice string = "abcdef";
  EXPECT_TRUE(string.endsWith("f"));
  EXPECT_TRUE(string.endsWith("abcdef"));
  EXPECT_FALSE(string.endsWith("z"));
}
