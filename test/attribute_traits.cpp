#include "duke/attributes/AttributeTraits.hpp"

#include <gtest/gtest.h>

template <typename T>
void checkRuntimeType(attribute::Type type) {
  EXPECT_EQ((attribute::runtime_type<T>()), type);
}

TEST(AttributeTraits, checkTypes) {
  checkRuntimeType<int8_t>(attribute::Type::Int8);
  checkRuntimeType<uint8_t>(attribute::Type::UInt8);
  checkRuntimeType<std::vector<uint8_t>>(attribute::Type::UInt8Array);
  checkRuntimeType<double>(attribute::Type::Float64);
  checkRuntimeType<Slice<float>>(attribute::Type::Float32Array);
  checkRuntimeType<std::string>(attribute::Type::String);
  checkRuntimeType<const char*>(attribute::Type::String);
}

template <typename T>
void checkUntypeRetypeValue(const T a) {
  EXPECT_EQ(a, attribute::asValue<T>(attribute::asMemorySlice(a)));
}

TEST(AttributeTraits, UntypeRetype) {
  checkUntypeRetypeValue<bool>(1);
  checkUntypeRetypeValue<uint8_t>(1);
  checkUntypeRetypeValue<uint16_t>(1);
  checkUntypeRetypeValue<uint32_t>(1);
  checkUntypeRetypeValue<uint64_t>(1);
  checkUntypeRetypeValue<int8_t>(1);
  checkUntypeRetypeValue<int16_t>(1);
  checkUntypeRetypeValue<int32_t>(1);
  checkUntypeRetypeValue<int64_t>(1);
  checkUntypeRetypeValue<float>(1);
  checkUntypeRetypeValue<double>(1);
}

template <typename C1, typename C2>
bool equals(const C1& c1, const C2& c2) {
  return c1.size() == c2.size() && std::equal(c1.begin(), c1.end(), c2.begin());
}

template <typename T>
void checkUntypeRetypeSlice(const T a) {
  {
    const std::initializer_list<T> list = {a, a, a};
    const Slice<const T> slice = attribute::asArray<T>(attribute::asMemorySlice(list));
    EXPECT_TRUE(equals(list, slice));
  }
  {
    const std::vector<T> list(a, 10);
    const Slice<const T> slice = attribute::asArray<T>(attribute::asMemorySlice(list));
    EXPECT_TRUE(equals(list, slice));
  }
}

TEST(AttributeTraits, UntypeRetypeSlice) {
  // bool is tested separately as vector<bool> is specialized and leads to compilation errors
  const std::initializer_list<bool> list = {false, true};
  const Slice<const bool> slice = attribute::asArray<const bool>(attribute::asMemorySlice(list));
  EXPECT_TRUE(equals(list, slice));
  // testing all other types
  checkUntypeRetypeSlice<uint8_t>(1);
  checkUntypeRetypeSlice<uint16_t>(1);
  checkUntypeRetypeSlice<uint32_t>(1);
  checkUntypeRetypeSlice<uint64_t>(1);
  checkUntypeRetypeSlice<int8_t>(1);
  checkUntypeRetypeSlice<int16_t>(1);
  checkUntypeRetypeSlice<int32_t>(1);
  checkUntypeRetypeSlice<int64_t>(1);
  checkUntypeRetypeSlice<float>(1);
  checkUntypeRetypeSlice<double>(1);
}

TEST(AttributeTraits, CStringUntyping) {
  const char pSrcString[] = "";
  const auto slice = attribute::asMemorySlice(pSrcString);
  EXPECT_EQ(slice.size(), 1);
  EXPECT_EQ(reinterpret_cast<const char*>(slice.begin()), pSrcString);
  EXPECT_EQ(*slice.begin(), '\0');
}

TEST(AttributeTraits, CStringRetyping) {
  const char pSrcString[] = "";
  const auto slice = attribute::asMemorySlice(pSrcString);
  const char* pDestString = attribute::asValue<const char*>(slice);
  EXPECT_EQ(pSrcString, pDestString);
  EXPECT_STREQ(pSrcString, pDestString);
}

TEST(AttributeTraits, StringUntyping) {
  const std::string srcString = "";
  const auto slice = attribute::asMemorySlice(srcString);
  EXPECT_EQ(slice.size(), 1);
  EXPECT_EQ(*slice.begin(), '\0');
}

TEST(AttributeTraits, StringRetyping) {
  const std::string srcString = "";
  const auto slice = attribute::asMemorySlice(srcString);
  const std::string destString = attribute::asValue<std::string>(slice);
  EXPECT_EQ(srcString, destString);
}
