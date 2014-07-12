#include <gtest/gtest.h>

#include "duke/base/StringTemplate.hpp"

TEST(StringTemplate, Instantiation) {
  const StringTemplate strTemplate("I can speak #language# !");
  BufferStringAppender<256> buffer;
  std::map<StringSlice, StringSlice> map;
  map["language"] = "French";
  strTemplate.instantiate(map, buffer);
  EXPECT_STREQ("I can speak French !", buffer.c_str());
}

TEST(StringTemplate, TagStartAndEndTemplate) {
  const StringTemplate strTemplate("#language#");
  BufferStringAppender<256> buffer;
  std::map<StringSlice, StringSlice> map;
  map["language"] = "French";
  strTemplate.instantiate(map, buffer);
  EXPECT_STREQ("French", buffer.c_str());
}

TEST(StringTemplate, CantInstantiate) {
  const StringTemplate strTemplate("I can speak #language# !");
  BufferStringAppender<256> buffer;
  std::map<StringSlice, StringSlice> map;
  EXPECT_DEATH(strTemplate.instantiate(map, buffer), "");
}
