#include <gtest/gtest.h>

#include <duke/base/StringUtils.hpp>

#include <string>
#include <vector>

using namespace std;

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
    string tmp;
    appendPaddedFrameNumber(1234, i, tmp);
    EXPECT_EQ(results.at(i), tmp);
  }
}

TEST(StringUtils, streq) {
  const char string[] = "string";
  EXPECT_TRUE(streq(string, string));
  EXPECT_TRUE(streq("", ""));
  EXPECT_TRUE(streq("a", "a"));
  EXPECT_FALSE(streq("a", "b"));

  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(streq(nullptr, nullptr),"");
  EXPECT_DEATH(streq(nullptr, "a"),"");
  EXPECT_DEATH(streq("a", nullptr),"");
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

  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH(strless(nullptr, nullptr),"");
  EXPECT_DEATH(strless(nullptr, "a"),"");
  EXPECT_DEATH(strless("a", nullptr),"");
}
