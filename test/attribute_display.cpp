#include "duke/attributes/Attribute.hpp"
#include "duke/attributes/AttributeDisplay.hpp"

#include <gtest/gtest.h>

TEST(AttributeDisplay, ConstCharData) {
  attribute::Attribute attribute("key", "hello");
  BufferStringAppender<1024> appender;
  attribute::append(attribute, appender);
  EXPECT_STREQ("\"hello\"", appender.c_str());
}

TEST(AttributeDisplay, IntVector) {
  const auto value = {1, 2, 3, 4};
  attribute::Attribute attribute("key", value);
  BufferStringAppender<1024> appender;
  attribute::append(attribute, appender);
  EXPECT_STREQ("[1,2,3,4]", appender.c_str());
}

TEST(AttributeDisplay, DoubleVector) {
  const auto value = {1., 2., 3., 4.};
  attribute::Attribute attribute("key", value);
  BufferStringAppender<1024> appender;
  attribute::append(attribute, appender);
  EXPECT_STREQ("[1.000000,2.000000,3.000000,4.000000]", appender.c_str());
}

TEST(AttributeDisplay, DoubleData) {
  attribute::Attribute attribute("key", 1.);
  BufferStringAppender<1024> appender;
  attribute::append(attribute, appender);
  EXPECT_STREQ("1.000000", appender.c_str());
}

TEST(AttributeDisplay, CharData) {
  attribute::Attribute attribute("key", uint8_t(5));
  BufferStringAppender<1024> appender;
  attribute::append(attribute, appender);
  EXPECT_STREQ("5", appender.c_str());
}

TEST(AttributeDisplay, UnsignedCharData) {
  attribute::Attribute attribute("key", (unsigned char)(20));
  BufferStringAppender<1024> appender;
  attribute::append(attribute, appender);
  EXPECT_STREQ("20", appender.c_str());
}
