#include <gtest/gtest.h>

#include "duke/base/StringTemplate.hpp"

TEST(StringTemplate, Instantiation) {
  const StringTemplate strTemplate("I can speak #language# !");
  BufferStringAppender<256> buffer;
  strTemplate.instantiate({{"language", "French"}}, buffer);
  EXPECT_STREQ("I can speak French !", buffer.c_str());
}

TEST(StringTemplate, TagStartAndEndTemplate) {
  const StringTemplate strTemplate("#language#");
  BufferStringAppender<256> buffer;
  strTemplate.instantiate({{"language", "French"}}, buffer);
  EXPECT_STREQ("French", buffer.c_str());
}

TEST(StringTemplate, CantInstantiate) {
  const StringTemplate strTemplate("I can speak #language# !");
  BufferStringAppender<256> buffer;
  EXPECT_DEATH(strTemplate.instantiate({}, buffer), "");
}
