#include <duke/attributes/Attributes.hpp>

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
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
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
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
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
