#include <duke/attributes/Attribute.hpp>
#include <duke/attributes/AttributeDisplay.hpp>

#include <gtest/gtest.h>

TEST(StringAppender, Full) {
  char buffer[1];
  StringAppender appender(buffer);
  EXPECT_FALSE(appender);        // can't output more data
  EXPECT_TRUE(appender.full());  // can't output more data
  EXPECT_EQ('\0', buffer[0]);
}

TEST(StringAppender, Appendable) {
  char buffer[2] = {'#', '#'};
  StringAppender appender(buffer);
  EXPECT_TRUE(appender);
  EXPECT_FALSE(appender.full());
  EXPECT_EQ('\0', buffer[0]);
  appender.append("2");
  EXPECT_FALSE(appender);        // can't output more data
  EXPECT_TRUE(appender.full());  // can't output more data
  EXPECT_EQ('2', buffer[0]);
  EXPECT_EQ('\0', buffer[1]);
}

TEST(StringAppender, Counting) {
  char buffer[5];
  StringAppender appender(buffer);
  while (appender) appender.append("XX");
  EXPECT_STREQ("XXXX", buffer);
}

TEST(StringAppender, WritingPast) {
  char buffer[2];
  StringAppender appender(buffer);
  appender.append("Hello!");
  EXPECT_STREQ("H", buffer);
}

TEST(BufferStringAppender, Appendable) {
  BufferStringAppender<2> appender;
  EXPECT_TRUE(appender);
  EXPECT_FALSE(appender.full());
  EXPECT_EQ('\0', appender.c_str()[0]);
  appender.append("2");
  EXPECT_FALSE(appender);        // can't output more data
  EXPECT_TRUE(appender.full());  // can't output more data
  EXPECT_EQ('2', appender.c_str()[0]);
  EXPECT_EQ('\0', appender.c_str()[1]);
}
