#include <duke/attributes/Attribute.hpp>
#include <duke/attributes/AttributeDisplay.hpp>

#include <algorithm>

#include <gtest/gtest.h>

TEST(Attribute, ForcedConstCharStar) {
  Attribute attribute;
  const char* const value = "value";
  attribute.set<const char*>("key", value);
  ASSERT_EQ(value, attribute.asValue<const char*>());
}

TEST(Attribute, CStringAllocates) {
  Attribute attribute;
  const char* const value = "value";
  attribute.set("key", value);
  ASSERT_STREQ(value, attribute.asCString());
  ASSERT_NE(value, attribute.asCString()) << "attribute allocated memory";
}

TEST(Attribute, String) {
  Attribute attribute;
  const std::string value = "value";
  attribute.set("key", value);
  auto str = attribute.asString();
  ASSERT_EQ(value, str);
}

template <typename Container1, typename Container2>
bool equal(const Container1& container1, const Container2& container2) {
  if (container1.size() != container2.size()) return false;
  return std::equal(std::begin(container1), std::end(container1),
                    std::begin(container2));
}

TEST(Attribute, Collection) {
  const auto value = {1, 2, 3, 4};
  Attribute attribute;
  attribute.set("key", value);
  ASSERT_TRUE(equal(value, attribute.asSlice<int>()));
  ASSERT_TRUE(equal(value, attribute.asVector<int>()));
}

struct Dummy {
  int a;
  double b;
  const char* ptr;

  bool operator==(const Dummy& other) const {
    return a == other.a && b == other.b && ptr == other.ptr;
  }
};

TEST(Attribute, Struct) {
  const Dummy value = {1, 2.0, nullptr};
  Attribute attribute;
  attribute.set("key", value);
  ASSERT_EQ(value, attribute.asValue<Dummy>());
}

TEST(Attribute, CollectionOfStruct) {
  const char* const ptr = "hello";
  const std::vector<Dummy> value = {{1, 2.0, nullptr}, {3, 4.0, ptr}};
  Attribute attribute;
  attribute.set("key", value);
  ASSERT_TRUE(equal(value, attribute.asSlice<Dummy>()));
  ASSERT_TRUE(equal(value, attribute.asVector<Dummy>()));
}

TEST(Attribute, MoveAssign) {
  Attribute attribute;
  attribute.set("key", {1});
  Attribute moved;
  moved = std::move(attribute);
  ASSERT_TRUE(attribute.value.empty());
  ASSERT_FALSE(moved.value.empty());
}

TEST(Attribute, Move) {
  Attribute attribute;
  attribute.set("key", {1});
  Attribute moved = std::move(attribute);
  ASSERT_TRUE(attribute.value.empty());
  ASSERT_FALSE(moved.value.empty());
}

TEST(Attribute, Swap) {
  const char* aName = "a";
  const char* bName = "b";
  Attribute a, b;
  a.set(aName, 'a');
  b.set(bName, "this is b");
  std::swap<Attribute>(a,b);
  ASSERT_EQ(a.key.name, bName);
  ASSERT_STREQ(a.asCString(), "this is b");
  ASSERT_EQ(b.asValue<char>(), 'a');
}

TEST(AttributeDisplay, RawData) {
  Attribute attribute;
  attribute.set("key", "hello");
  AttributeDisplay display;
  {
    char buf[1024];
    display.snprintf(attribute, buf, sizeof(buf));
    EXPECT_STREQ("68-65-6C-6C-6F-00", buf);
  }
  {
    char buf[1];
    display.snprintf(attribute, buf, sizeof(buf));
    EXPECT_STREQ("", buf);
  }
  {
    char buf[5];
    display.snprintf(attribute, buf, sizeof(buf));
    EXPECT_STREQ("6...", buf);
  }
  {
    char buf[7];
    display.snprintf(attribute, buf, sizeof(buf));
    EXPECT_STREQ("68-...", buf);
  }
}

TEST(AttributeDisplay, ConstCharData) {
  Attribute attribute;
  attribute.set("key", "hello");
  AttributeDisplay display;
  display.registerType<char>();
  char buf[1024];
  display.snprintf(attribute, buf, sizeof(buf));
  EXPECT_STREQ(R"("hello")", buf);
}

TEST(AttributeDisplay, IntVector) {
  const auto value = {1, 2, 3, 4};
  Attribute attribute;
  attribute.set("key", value);
  AttributeDisplay display;
  display.registerType<int>();
  char buf[1024];
  display.snprintf(attribute, buf, sizeof(buf));
  EXPECT_STREQ("[1,2,3,4]", buf);
}

TEST(AttributeDisplay, DoubleVector) {
  const auto value = {1., 2., 3., 4.};
  Attribute attribute;
  attribute.set("key", value);
  AttributeDisplay display;
  display.registerType<double>();
  char buf[1024];
  display.snprintf(attribute, buf, sizeof(buf));
  EXPECT_STREQ("[1.000000,2.000000,3.000000,4.000000]", buf);
}

TEST(AttributeDisplay, DoubleData) {
  Attribute attribute;
  attribute.set("key", 1.);
  AttributeDisplay display;
  display.registerType<double>();
  char buf[1024];
  display.snprintf(attribute, buf, sizeof(buf));
  EXPECT_STREQ("1.000000", buf);
}

TEST(AttributeDisplay, CharData) {
  Attribute attribute;
  attribute.set("key", 'a');
  AttributeDisplay display;
  registerPrimitiveTypes(display);
  char buf[1024];
  display.snprintf(attribute, buf, sizeof(buf));
  EXPECT_STREQ("'a'", buf);
}

TEST(AttributeDisplay, UnsignedCharData) {
  Attribute attribute;
  attribute.set<unsigned char>("key", 20);
  AttributeDisplay display;
  registerPrimitiveTypes(display);
  char buf[1024];
  display.snprintf(attribute, buf, sizeof(buf));
  EXPECT_STREQ("20", buf);
}
