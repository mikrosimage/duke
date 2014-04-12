#include <duke/attributes/Attributes.hpp>

#include <gtest/gtest.h>

TEST(AttributePrototype, simple) {
  RegisterValueAttribute(double, kDouble, 0);
  EXPECT_STREQ("kDouble", kDouble::key());
  EXPECT_EQ(0, kDouble::default_value());
  EXPECT_TRUE((std::is_same<double, kDouble::value_type>::value));
}

TEST(AttributePrototype, array) {
  RegisterArrayAttribute(double, kDoubleArray, {});
  EXPECT_STREQ("kDoubleArray", kDoubleArray::key());
  EXPECT_TRUE((std::is_same<Slice<double>, kDoubleArray::value_type>::value));
}

//TEST(AttributePrototype, string) {
//  RegisterStringAttribute(kString, nullptr);
//  Attributes attributes;
//  EXPECT_EQ(nullptr, attributes.getOrDefault<kString>());
//  attributes.set<kString>(nullptr);
//  EXPECT_EQ(nullptr, attributes.getOrDefault<kString>());
//}

struct PlainOldData {
  double a;
  int b;
  bool operator==(const PlainOldData& other) const {
    return a == other.a && b == other.b;
  }
};

enum Enum {
  ONE,
  TWO
};

template <typename T>
void compare(const T& a, const T& b) {
  EXPECT_EQ(a, b);
}

template <>
void compare(const std::string& a, const std::string& b) {
  EXPECT_STREQ(a.c_str(), b.c_str());
}

template <>
void compare(const char* const& a, const char* const& b) {
  EXPECT_STREQ(a, b);
}
template <typename T>
void testSuite(const typename T::value_type& defaultValue,
               const typename T::value_type& value) {
  Attributes attributes;
  // no attributes
  EXPECT_FALSE(attributes.contains<T>());
  EXPECT_EQ(attributes.size(), 0);
  EXPECT_EQ(attributes.getOrDefault<T>(), T::default_value());
  EXPECT_EQ(attributes.getWithDefault<T>(defaultValue), defaultValue);
  // setting one attribute
  attributes.set<T>(value);
  EXPECT_EQ(attributes.size(), 1);
  EXPECT_TRUE(attributes.contains<T>());
  // setting two times will not add another value
  attributes.set<T>(value);
  EXPECT_EQ(attributes.size(), 1);
  // checking get will lead to correct result
  compare<typename T::value_type>(attributes.getOrDie<T>(), value);
  compare<typename T::value_type>(attributes.getOrDefault<T>(), value);
  compare<typename T::value_type>(attributes.getWithDefault<T>(defaultValue),
                                  value);
  // removing data
  attributes.erase<T>();
  EXPECT_EQ(attributes.size(), 0);
  EXPECT_FALSE(attributes.contains<T>());
}

TEST(Attributes, string) {
  RegisterStringAttribute(kString, "");
  testSuite<kString>("default", "value");
}

TEST(Attributes, integral) {
  RegisterValueAttribute(uint64_t, Uint64Attribute, 0LL);
  testSuite<Uint64Attribute>(0ULL, 123ULL);
}

TEST(Attributes, POD) {
  RegisterValueAttribute(PlainOldData, PODAttribute, PlainOldData());
  testSuite<PODAttribute>({0.0, 0}, {1.0, 1});
}

TEST(Attributes, enum) {
  RegisterValueAttribute(Enum, EnumAttribute, Enum::ONE);
  testSuite<EnumAttribute>(Enum::ONE, Enum::TWO);
}
