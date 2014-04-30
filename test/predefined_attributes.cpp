#include <duke/attributes/PredefinedAttribute.hpp>
#include <duke/attributes/AttributeTraits.hpp>

#include <gtest/gtest.h>

#include <iostream>

DECLARE_ATTRIBUTE(AttributeUInt32, uint32_t, "a", 10);
DECLARE_ATTRIBUTE(AttributeDouble, double, "b", 0);
DECLARE_ATTRIBUTE(AttributeString, const char*, "c", "default");
DECLARE_ARRAY_ATTRIBUTE(AttributeArray, float, "d");

using namespace attribute;

TEST(PredefinedAttributes, Empty) {
  Attributes attributes;
  EXPECT_FALSE(contains<AttributeUInt32>(attributes));
  EXPECT_EQ(2, getWithDefault<AttributeUInt32>(attributes, 2));  // return default key
  erase<AttributeUInt32>(attributes);                            // legal to delete an unknown key
}

TEST(PredefinedAttributes, ConstEmpty) {
  const Attributes attributes;
  EXPECT_FALSE(contains<AttributeUInt32>(attributes));
  EXPECT_EQ(10, getWithDefault<AttributeUInt32>(attributes));    // return default key
  EXPECT_EQ(2, getWithDefault<AttributeUInt32>(attributes, 2));  // return default key
}

TEST(PredefinedAttributes, ConstOneElement) {
  const Attributes attributes = {Attribute(AttributeUInt32::key, 1u)};
  EXPECT_TRUE(contains<AttributeUInt32>(attributes));
  EXPECT_EQ(1u, getWithDefault<AttributeUInt32>(attributes, 2));
  EXPECT_EQ(1u, getOrDie<AttributeUInt32>(attributes));
}

TEST(PredefinedAttributes, Writable) {
  Attributes attributes = {Attribute(AttributeUInt32::key, 1u)};
  EXPECT_TRUE(contains<AttributeUInt32>(attributes));
  EXPECT_EQ(1, getOrDie<AttributeUInt32>(attributes));
  // set a new key
  set<AttributeDouble>(attributes, 5.);
  EXPECT_TRUE(contains<AttributeUInt32>(attributes));
  EXPECT_TRUE(contains<AttributeDouble>(attributes));
  EXPECT_EQ(5., getOrDie<AttributeDouble>(attributes));
  // delete first key
  erase<AttributeUInt32>(attributes);
  EXPECT_FALSE(contains<AttributeUInt32>(attributes));
  EXPECT_TRUE(contains<AttributeDouble>(attributes));
  // overwrite value
  set<AttributeDouble>(attributes, 2.);
  EXPECT_EQ(2., getOrDie<AttributeDouble>(attributes));
  // reset key
  EXPECT_TRUE(setIfNotPresent<AttributeUInt32>(attributes, 5));
  EXPECT_TRUE(contains<AttributeUInt32>(attributes));
  // can't reset it again
  EXPECT_FALSE(setIfNotPresent<AttributeUInt32>(attributes, 5));
}

TEST(PredefinedAttributes, String) {
  Attributes attributes;
  EXPECT_FALSE(contains<AttributeString>(attributes));
  set<AttributeString>(attributes, "hola!");
  EXPECT_TRUE(contains<AttributeString>(attributes));
  EXPECT_STREQ("hola!", getOrDie<AttributeString>(attributes));
  EXPECT_STREQ("hola!", getWithDefault<AttributeString>(attributes));
  attributes.clear();
  EXPECT_STREQ("default", getWithDefault<AttributeString>(attributes));
}

TEST(PredefinedAttributes, FloatArray) {
  Attributes attributes;
  EXPECT_FALSE(contains<AttributeArray>(attributes));
  set<AttributeArray>(attributes, std::vector<float>{2.f, 3.f});
  EXPECT_TRUE(contains<AttributeArray>(attributes));
  EXPECT_EQ(asSlice({2.f, 3.f}), getOrDie<AttributeArray>(attributes));
  erase<AttributeArray>(attributes);
  EXPECT_FALSE(contains<AttributeArray>(attributes));
  EXPECT_TRUE(setIfNotPresent<AttributeArray>(attributes, std::initializer_list<float>{1.f}));
  EXPECT_EQ(asSlice({1.f}), getOrDie<AttributeArray>(attributes));
  EXPECT_TRUE(contains<AttributeArray>(attributes));
  EXPECT_FALSE(setIfNotPresent<AttributeArray>(attributes, std::initializer_list<float>{2.f}));
  set<AttributeArray>(attributes, asSlice<float>({3.f}));
  EXPECT_EQ(asSlice({3.f}), getOrDie<AttributeArray>(attributes));
}

TEST(PredefinedAttributesDeathTest, GetOrDie) {
  Attributes attributes;
  EXPECT_DEATH(getOrDie<AttributeUInt32>(attributes), "");  // dies
}

TEST(PredefinedAttributesDeathTest, ConstGetOrDie) {
  const Attributes attributes;
  EXPECT_DEATH(getOrDie<AttributeUInt32>(attributes), "");  // dies
}

TEST(PredefinedAttributesDeathTest, TryToGetInvalidType) {
  const Attributes attributes = {Attribute(AttributeUInt32::key, 1.0)};  // Float64
  EXPECT_DEATH(contains<AttributeUInt32>(attributes), "");
  EXPECT_DEATH(getWithDefault<AttributeUInt32>(attributes, 2u), "");
  EXPECT_DEATH(getOrDie<AttributeUInt32>(attributes), "");
}
