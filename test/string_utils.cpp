#include <gtest/gtest.h>

#include "duke/base/StringAppender.hpp"
#include "duke/base/StringSlice.hpp"
#include "duke/base/StringUtils.hpp"

#include <string>
#include <vector>

using namespace std;

void PrintTo(const std::vector<StringSlice>& slices, ::std::ostream* os) {
  *os << "[ ";
  bool first = true;
  for (const auto& slice : slices) {
    if (!first) *os << ", ";
    *os << slice;
    first = false;
  }
  *os << " ]";
}

TEST(StringUtils, digits) {
  EXPECT_EQ(1, digits(0));
  EXPECT_EQ(1, digits(1));
  EXPECT_EQ(1, digits(9));
  EXPECT_EQ(2, digits(10));
  EXPECT_EQ(2, digits(99));
  EXPECT_EQ(9, digits(123456789));
}

TEST(StringUtils, append) {
  const vector<string> results = {"", "4", "34", "234", "1234", "01234", "001234", "0001234", "00001234", "000001234"};
  for (size_t i = 0; i < results.size(); ++i) {
    BufferStringAppender<32> tmp;
    appendPaddedFrameNumber(1234, i, tmp);
    EXPECT_EQ(results.at(i), tmp.c_str());
  }
}

TEST(StringUtils, streq) {
  const char string[] = "string";
  EXPECT_TRUE(streq(string, string));
  EXPECT_TRUE(streq("", ""));
  EXPECT_TRUE(streq("a", "a"));
  EXPECT_FALSE(streq("a", "b"));

  EXPECT_DEATH(streq(nullptr, nullptr), "");
  EXPECT_DEATH(streq(nullptr, "a"), "");
  EXPECT_DEATH(streq("a", nullptr), "");
}

TEST(StringUtils, strless) {
  const char string[] = "string";
  EXPECT_TRUE(strless("a", "b"));
  EXPECT_FALSE(strless("b", "a"));
  EXPECT_FALSE(strless(string, string));
  EXPECT_FALSE(strless("", ""));
  EXPECT_FALSE(strless("a", "a"));
  EXPECT_TRUE(strless("a", "aa"));
  EXPECT_FALSE(strless("aa", "a"));

  EXPECT_DEATH(strless(nullptr, nullptr), "");
  EXPECT_DEATH(strless(nullptr, "a"), "");
  EXPECT_DEATH(strless("a", nullptr), "");
}

void checkDimensions(const char* string, unsigned expectedWidth, unsigned expectedHeight) {
  unsigned width, height;
  strdim(string, width, height);
  EXPECT_EQ(expectedWidth, width);
  EXPECT_EQ(expectedHeight, height);
}

TEST(StringUtils, strdim) {
  checkDimensions("", 0, 0);
  checkDimensions("1", 1, 0);
  checkDimensions("\n", 0, 1);
  checkDimensions("_\n_", 1, 1);
  checkDimensions("__\n", 2, 1);
  checkDimensions("__\n___\n", 3, 2);
}

TEST(StringSlice, stripPrefix) {
  StringSlice string = "abcdef";
  EXPECT_FALSE(stripPrefix("c", string));
  EXPECT_TRUE(stripPrefix("a", string));
  EXPECT_EQ(string, "bcdef");
}

TEST(StringSlice, stripSuffix) {
  StringSlice string = "abcdef";
  EXPECT_FALSE(stripSuffix("c", string));
  EXPECT_TRUE(stripSuffix("f", string));
  EXPECT_EQ(string, "abcde");
}

TEST(StringSlice, find) {
  EXPECT_EQ(std::string::npos, find("abcde", "f"));
  EXPECT_EQ(0, find("abcde", "a"));
  EXPECT_EQ(1, find("abcde", "b"));
}

typedef std::vector<StringSlice> StringSlices;

TEST(StringSlice, splitEmpty) {
  EXPECT_EQ(StringSlices{}, split("", ','));
  EXPECT_EQ(StringSlices{}, split("", ","));
}

TEST(StringSlice, splitNone) {
  EXPECT_EQ(StringSlices{"abc"}, split("abc", ','));
  EXPECT_EQ(StringSlices{"abc"}, split("abc", "__"));
}

TEST(StringSlice, splitTwo) {
  EXPECT_EQ(StringSlices({"ab", "c"}), split("ab,c", ','));
  EXPECT_EQ(StringSlices({"ab", "c"}), split("ab__c", "__"));
}

TEST(StringSlice, splitThree) {
  EXPECT_EQ(StringSlices({"a", "b", "c"}), split("a,b,c", ','));
  EXPECT_EQ(StringSlices({"a", "b", "c"}), split("a__b__c", "__"));
}

TEST(StringSlice, trimEmpty) { EXPECT_EQ(StringSlice(""), trim("")); }

TEST(StringSlice, trimOnlySpaces) { EXPECT_EQ(StringSlice(""), trim("   ")); }

TEST(StringSlice, trimFront) { EXPECT_EQ(StringSlice("a"), trim(" a")); }

TEST(StringSlice, trimBack) { EXPECT_EQ(StringSlice("a"), trim("a  ")); }
