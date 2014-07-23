#include "duke/attributes/Attribute.hpp"
#include "duke/attributes/AttributeDisplay.hpp"

#include <gtest/gtest.h>

TEST(StringAppender, Full) {
  char buffer[1];
  StringAppender appender(buffer);
  EXPECT_FALSE(appender);        // can't output more data
  EXPECT_TRUE(appender.full());  // can't output more data
  EXPECT_EQ('\0', buffer[0]);
}

TEST(StringAppender, Size) {
  char buffer[2];
  StringAppender appender(buffer);
  EXPECT_EQ(0, appender.size());
  appender.append("#");
  EXPECT_EQ(1, appender.size());
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
  EXPECT_EQ(StringSlice(), appender.c_str());
  appender.append("2");
  EXPECT_FALSE(appender);        // can't output more data
  EXPECT_TRUE(appender.full());  // can't output more data
  EXPECT_EQ('2', appender.c_str()[0]);
  EXPECT_EQ('\0', appender.c_str()[1]);
  EXPECT_EQ(StringSlice("2"), appender.slice());
}

TEST(StringAppender, digits10) {
  BufferStringAppender<10> appender;
  appender.appendInteger(10);
  EXPECT_EQ(StringSlice("10"), appender.slice());
}

TEST(StringAppender, digits16) {
  BufferStringAppender<10> appender;
  appender.appendInteger(255, 16);
  EXPECT_EQ(StringSlice("FF"), appender.slice());
}

TEST(StringAppender, digits2) {
  BufferStringAppender<10> appender;
  appender.appendInteger(5, 2);
  EXPECT_EQ(StringSlice("101"), appender.slice());
}
