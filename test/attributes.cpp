#include "duke/attributes/Attributes.hpp"

#include <gtest/gtest.h>

static const char kKey[] = "key";
static const char kAnotherKey[] = "another_key";

using namespace attribute;

TEST(Attributes, Empty) {
  Attributes attributes;
  EXPECT_EQ(attributes.end(), find(attributes, kKey));
  EXPECT_FALSE(contains(attributes, kKey));
  EXPECT_EQ(Attribute(), get(attributes, kKey));  // return default key
  erase(attributes, kKey);                        // legal to delete an unknown key
}

TEST(AttributesDeathTest, GetOrDie) {
  Attributes attributes;
  ASSERT_DEATH(getOrDie(attributes, kKey), "");  // dies
}

TEST(Attributes, ConstEmpty) {
  const Attributes attributes;
  EXPECT_EQ(attributes.end(), find(attributes, kKey));
  EXPECT_FALSE(contains(attributes, kKey));
  EXPECT_EQ(Attribute(), get(attributes, kKey));  // return default key
}

TEST(AttributesDeathTest, ConstGetOrDie) {
  const Attributes attributes;
  ASSERT_DEATH(getOrDie(attributes, kKey), "");  // dies
}

TEST(Attributes, ConstOneElement) {
  const Attributes attributes = {Attribute(kKey, 1.0)};
  EXPECT_EQ(attributes.begin(), find(attributes, kKey));
  EXPECT_TRUE(contains(attributes, kKey));
  EXPECT_EQ(attributes.front(), get(attributes, kKey));
  EXPECT_EQ(attributes.front(), getOrDie(attributes, kKey));
}

TEST(Attributes, Writable) {
  Attributes attributes = {Attribute(kKey, 1.0)};
  EXPECT_EQ(attributes.begin(), find(attributes, kKey));
  EXPECT_TRUE(contains(attributes, kKey));
  EXPECT_EQ(Type::Float64, get(attributes, kKey).type);
  EXPECT_EQ(attributes.front(), get(attributes, kKey));
  // set a new key
  set(attributes, kAnotherKey, 5);
  EXPECT_TRUE(contains(attributes, kKey));
  EXPECT_TRUE(contains(attributes, kAnotherKey));
  EXPECT_EQ(Type::Int32, get(attributes, kAnotherKey).type);
  // delete first key
  erase(attributes, kKey);
  EXPECT_FALSE(contains(attributes, kKey));
  EXPECT_TRUE(contains(attributes, kAnotherKey));
  // overwrite value
  set(attributes, kAnotherKey, 2.f);
  EXPECT_EQ(Type::Float32, get(attributes, kAnotherKey).type);
  // reset key
  EXPECT_TRUE(setIfNotPresent(attributes, kKey, 5));
  EXPECT_TRUE(contains(attributes, kKey));
  // can't reset it again
  EXPECT_FALSE(setIfNotPresent(attributes, kKey, 5));
}

TEST(Attributes, MergeNoConflict) {
  const Attributes from = {Attribute("c", 3), Attribute("d", "4")};
  Attributes to = {Attribute("a", 1.0), Attribute("b", uint64_t(2))};
  merge(from, to);
  ASSERT_EQ(4, to.size());
  EXPECT_EQ(Attribute("a", 1.0), to.at(0));
  EXPECT_EQ(Attribute("b", uint64_t(2)), to.at(1));
  EXPECT_EQ(Attribute("c", 3), to.at(2));
  EXPECT_EQ(Attribute("d", "4"), to.at(3));
}

TEST(Attributes, MergeOverride) {
  const Attributes from = {Attribute("b", 3), Attribute("c", "4")};
  Attributes to = {Attribute("a", 1.0), Attribute("b", uint64_t(2))};
  merge(from, to);
  ASSERT_EQ(3, to.size());
  EXPECT_EQ(Attribute("a", 1.0), to.at(0));
  EXPECT_EQ(Attribute("b", 3), to.at(1));
  EXPECT_EQ(Attribute("c", "4"), to.at(2));
}
