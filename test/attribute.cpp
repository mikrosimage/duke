#include <duke/attributes/Attribute.hpp>

#include <gtest/gtest.h>

std::ostream& operator<<(std::ostream& stream, attribute::Type type) {
  return stream << attribute::getTypeString(type);
}

template <typename A, typename B>
size_t offset(const A& a, const B& b) {
  return reinterpret_cast<size_t>(&a) - reinterpret_cast<size_t>(&b);
}

TEST(Attribute, Layout) {
  EXPECT_EQ(4, sizeof(attribute::Type));
  EXPECT_EQ(48, sizeof(attribute::Attribute::value_type));
  EXPECT_EQ(64, sizeof(attribute::Attribute));
  attribute::Attribute attribute;
  EXPECT_EQ(0, offset(attribute.name, attribute));
  EXPECT_EQ(8, offset(attribute.type, attribute));
  EXPECT_EQ(16, offset(attribute.value, attribute));
}

TEST(Attribute, Default) {
  attribute::Attribute attribute;
  EXPECT_EQ(nullptr, attribute.name);
  EXPECT_EQ(attribute::Type::Invalid, attribute.type);
  const ConstMemorySlice slice = attribute;
  EXPECT_TRUE(slice.empty());
}

const char kKey[] = "key";

template <typename T>
void checkUntypeRetypePrimitive() {
  const T value(1);
  const attribute::Attribute attribute(kKey, value);
  EXPECT_EQ(kKey, attribute.name);
  EXPECT_EQ(sizeof(T), attribute.value.size());
  EXPECT_NE(attribute::Type::Invalid, attribute.type);
  EXPECT_EQ(attribute::runtime_type<T>(), attribute.type);
  EXPECT_EQ(attribute::runtime_type<const T>(), attribute.type);
  EXPECT_EQ(value, attribute::asValue<T>(attribute));
}

TEST(Attribute, CheckUntypeRetype) {
  checkUntypeRetypePrimitive<bool>();
  checkUntypeRetypePrimitive<uint8_t>();
  checkUntypeRetypePrimitive<uint16_t>();
  checkUntypeRetypePrimitive<uint32_t>();
  checkUntypeRetypePrimitive<uint64_t>();
  checkUntypeRetypePrimitive<int8_t>();
  checkUntypeRetypePrimitive<int16_t>();
  checkUntypeRetypePrimitive<int32_t>();
  checkUntypeRetypePrimitive<int64_t>();
  checkUntypeRetypePrimitive<float>();
  checkUntypeRetypePrimitive<double>();
}

template <typename C1, typename C2>
bool equals(const C1& c1, const C2& c2) {
  return c1.size() == c2.size() && std::equal(c1.begin(), c1.end(), c2.begin());
}

template <typename T>
void checkUntypeRetypeArray() {
  const std::initializer_list<const T> value = {0, 1};
  const attribute::Attribute attribute(kKey, value);
  EXPECT_EQ(kKey, attribute.name);
  EXPECT_EQ(sizeof(T) * 2, attribute.value.size());
  EXPECT_NE(attribute::Type::Invalid, attribute.type);
  EXPECT_EQ(attribute::runtime_type<std::initializer_list<const T>>(), attribute.type);
  EXPECT_EQ(attribute::runtime_type<const std::initializer_list<const T>>(), attribute.type);
  EXPECT_TRUE(equals(value, attribute::asArray<T>(attribute)));
}

TEST(Attribute, CheckUntypeRetypeArray) {
  checkUntypeRetypeArray<bool>();
  checkUntypeRetypeArray<uint8_t>();
  checkUntypeRetypeArray<uint16_t>();
  checkUntypeRetypeArray<uint32_t>();
  checkUntypeRetypeArray<uint64_t>();
  checkUntypeRetypeArray<int8_t>();
  checkUntypeRetypeArray<int16_t>();
  checkUntypeRetypeArray<int32_t>();
  checkUntypeRetypeArray<int64_t>();
  checkUntypeRetypeArray<float>();
  checkUntypeRetypeArray<double>();
}

template <typename T>
void checkUntypeRetypeVector() {
  const std::vector<T> value = {0, 1, 2};
  const attribute::Attribute attribute(kKey, value);
  EXPECT_EQ(kKey, attribute.name);
  EXPECT_EQ(sizeof(T) * 3, attribute.value.size());
  EXPECT_NE(attribute::Type::Invalid, attribute.type);
  EXPECT_EQ(attribute::runtime_type<std::vector<const T>>(), attribute.type);
  EXPECT_EQ(attribute::runtime_type<const std::vector<const T>>(), attribute.type);
  EXPECT_TRUE(equals(value, attribute::asArray<T>(attribute)));
}

TEST(Attribute, CheckUntypeRetypeVector) {
  // no vector<bool> which is specialized and can't be used
  checkUntypeRetypeVector<uint8_t>();
  checkUntypeRetypeVector<uint16_t>();
  checkUntypeRetypeVector<uint32_t>();
  checkUntypeRetypeVector<uint64_t>();
  checkUntypeRetypeVector<int8_t>();
  checkUntypeRetypeVector<int16_t>();
  checkUntypeRetypeVector<int32_t>();
  checkUntypeRetypeVector<int64_t>();
  checkUntypeRetypeVector<float>();
  checkUntypeRetypeVector<double>();
}

TEST(Attribute, CheckCString) {
  const char* pString = "hello";
  const attribute::Attribute attribute(kKey, pString);
  EXPECT_NE(pString, attribute::asValue<const char*>(attribute));
  EXPECT_STREQ(pString, attribute::asValue<const char*>(attribute));
}

TEST(Attribute, CheckString) {
  const std::string string = "hello";
  const attribute::Attribute attribute(kKey, string);
  EXPECT_EQ(string, attribute::asValue<std::string>(attribute));
}

TEST(Attribute, Copy) {
  const auto doubleAttribute = attribute::Attribute(kKey, 1.3);
  EXPECT_EQ(attribute::Type::Float64, doubleAttribute.type);
  auto stringAttribute = attribute::Attribute(kKey, std::string("string"));
  EXPECT_EQ(attribute::Type::String, stringAttribute.type);
  stringAttribute = doubleAttribute;
  EXPECT_EQ(attribute::Type::Float64, stringAttribute.type);
  EXPECT_EQ(1.3, attribute::asValue<double>(stringAttribute));
}

TEST(Attribute, Move) {
  auto doubleAttribute = attribute::Attribute(kKey, 1.4);
  EXPECT_EQ(attribute::Type::Float64, doubleAttribute.type);
  auto stringAttribute = attribute::Attribute(kKey, std::string("string"));
  EXPECT_EQ(attribute::Type::String, stringAttribute.type);
  stringAttribute = std::move(doubleAttribute);
  EXPECT_EQ(attribute::Type::Float64, stringAttribute.type);
  EXPECT_EQ(1.4, attribute::asValue<double>(stringAttribute));
  EXPECT_EQ(attribute::Type::Invalid, doubleAttribute.type);
  EXPECT_TRUE(doubleAttribute.value.empty());
}

// TEST(AttributeDisplay, RawData) {
//  Attribute attribute;
//  attribute.set("key", "hello");
//  AttributeDisplay display;
//  {
//    char buf[1024];
//    display.snprintf(attribute, buf, sizeof(buf));
//    EXPECT_STREQ("68-65-6C-6C-6F-00", buf);
//  }
//  {
//    char buf[1];
//    display.snprintf(attribute, buf, sizeof(buf));
//    EXPECT_STREQ("", buf);
//  }
//  {
//    char buf[5];
//    display.snprintf(attribute, buf, sizeof(buf));
//    EXPECT_STREQ("6...", buf);
//  }
//  {
//    char buf[7];
//    display.snprintf(attribute, buf, sizeof(buf));
//    EXPECT_STREQ("68-...", buf);
//  }
//}
//
// TEST(AttributeDisplay, ConstCharData) {
//  Attribute attribute;
//  attribute.set("key", "hello");
//  AttributeDisplay display;
//  registerPrimitiveTypes(display);
//  char buf[1024];
//  display.snprintf(attribute, buf, sizeof(buf));
//  EXPECT_STREQ(R"("hello")", buf);
//}
//
// TEST(AttributeDisplay, IntVector) {
//  const auto value = {1, 2, 3, 4};
//  Attribute attribute;
//  attribute.set("key", value);
//  AttributeDisplay display;
//  registerPrimitiveTypes(display);
//  char buf[1024];
//  display.snprintf(attribute, buf, sizeof(buf));
//  EXPECT_STREQ("[1,2,3,4]", buf);
//}
//
// TEST(AttributeDisplay, DoubleVector) {
//  const auto value = {1., 2., 3., 4.};
//  Attribute attribute;
//  attribute.set("key", value);
//  AttributeDisplay display;
//  registerPrimitiveTypes(display);
//  char buf[1024];
//  display.snprintf(attribute, buf, sizeof(buf));
//  EXPECT_STREQ("[1.000000,2.000000,3.000000,4.000000]", buf);
//}
//
// TEST(AttributeDisplay, DoubleData) {
//  Attribute attribute;
//  attribute.set("key", 1.);
//  AttributeDisplay display;
//  registerPrimitiveTypes(display);
//  char buf[1024];
//  display.snprintf(attribute, buf, sizeof(buf));
//  EXPECT_STREQ("1.000000", buf);
//}
//
// TEST(AttributeDisplay, CharData) {
//  Attribute attribute;
//  attribute.set("key", 'a');
//  AttributeDisplay display;
//  registerPrimitiveTypes(display);
//  char buf[1024];
//  display.snprintf(attribute, buf, sizeof(buf));
//  EXPECT_STREQ("'a'", buf);
//}
//
// TEST(AttributeDisplay, UnsignedCharData) {
//  Attribute attribute;
//  attribute.set<unsigned char>("key", 20);
//  AttributeDisplay display;
//  registerPrimitiveTypes(display);
//  char buf[1024];
//  display.snprintf(attribute, buf, sizeof(buf));
//  EXPECT_STREQ("20", buf);
//}
