#include <gtest/gtest.h>

#include <duke/attributes/Attribute.hpp>
#include <duke/attributes/Attributes.hpp>
#include <duke/attributes/AttributeDisplay.hpp>
#include <glm/glm.hpp>

using namespace std;

struct PlainOldData {
    double a;
    int b;
    bool operator==(const PlainOldData& other) const {
        return a == other.a && b == other.b;
    }
};

enum Enum {
    ONE, TWO
};

template<>
std::string TypedAttributeDescriptor<PlainOldData>::dataToString(const void* pData, size_t size) const {
    CHECK(sizeof(PlainOldData) == size);
    const PlainOldData pod = *reinterpret_cast<const PlainOldData*>(pData);
    std::string output;
    output += '[';
    output += std::to_string(pod.a);
    output += ',';
    output += std::to_string(pod.b);
    output += ']';
    return output;
}

template<>
std::string TypedAttributeDescriptor<Enum>::dataToString(const void* pData, size_t size) const {
    switch (*reinterpret_cast<const Enum*>(pData)) {
        case ONE:
            return "ONE";
        case TWO:
            return "TWO";
    }
    return {};
}

namespace attribute {

DECLARE_ATTRIBUTE(std::string, StringAttribute, "");
DECLARE_ATTRIBUTE(const char*, CStringAttribute, nullptr);
DECLARE_ATTRIBUTE(uint64_t, Uint64Attribute, 0);
DECLARE_ATTRIBUTE(PlainOldData, PODAttribute, { });
DECLARE_ATTRIBUTE(Enum, EnumAttribute, Enum::ONE);

} /* namespace attribute */

using namespace attribute;

template<typename T>
void compare(const T& a, const T& b) {
    EXPECT_EQ(a, b);
}

template<>
void compare(const char* const & a, const char* const & b) {
    EXPECT_STREQ(a, b);
}

template<typename T>
void testSuite(const typename T::value_type& defaultValue, const typename T::value_type& value, const string& typeStr, const string& valueStr) {
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
    // displaying type and value
    {
        const auto& entry = attributes.get<T>();
        EXPECT_EQ(typeString(entry), typeStr);
        EXPECT_EQ(dataString(entry), valueStr);
        EXPECT_STREQ(nameString(entry), T().name());
    }
    // checking get will lead to correct result
    compare<typename T::value_type>(attributes.getOrDie<T>(), value);
    compare<typename T::value_type>(attributes.getOrDefault<T>(), value);
    compare<typename T::value_type>(attributes.getWithDefault<T>(defaultValue), value);
    // removing data
    attributes.erase<T>();
    EXPECT_EQ(attributes.size(), 0);
    EXPECT_FALSE(attributes.contains<T>());
    // displaying empty data
    {
        const auto& entry = attributes.get<T>();
        EXPECT_EQ(dataString(entry), "N/A");
        EXPECT_STREQ(nameString(entry), "N/A");
    }
}

TEST(Attributes, string) {
    testSuite<StringAttribute>("default", "value", "std::string", "value");
}

TEST(Attributes, cstring) {
    testSuite<CStringAttribute>("default", "value", "const char*", "value");
    Attributes attributes;
}

TEST(Attributes, integral) {
    testSuite<Uint64Attribute>(0ULL, 123ULL, "uint64_t", "123");
}

TEST(Attributes, POD) {
    testSuite<PODAttribute>( { 0.0, 0 }, { 1.0, 1 }, "PlainOldData", "[1.000000,1]");
}

TEST(Attributes, enum) {
    testSuite<EnumAttribute>(Enum::ONE, Enum::TWO, "Enum", "TWO");
}

TEST(Attributes, RawAtributeData) {
    const char pKey[] = "hello";
    Attributes attributes;
    // empty
    EXPECT_FALSE(attributes.contains(pKey));
    EXPECT_EQ(attributes.size(), 0);
    // set
    const char pData[] = "rawdata";
    const size_t dataSize = sizeof(pData);
    attributes.set(pKey, nullptr, pData, dataSize);
    // one value
    EXPECT_EQ(attributes.size(), 1);
    // accessible by one key
    EXPECT_TRUE(attributes.contains(pKey));
    // or with another string but same value
    const char pAnotherEqualKey[] = "hello";
    EXPECT_NE(pKey, pAnotherEqualKey) << "keys are not the same";
    EXPECT_TRUE(attributes.contains(pAnotherEqualKey));
    // fetching raw data
    const auto value = attributes.getOrDie(pKey);
    EXPECT_EQ(value.data.size(), dataSize) << "same size";
    EXPECT_NE(value.data.ptr(), pData) << "memory should be allocated";
    EXPECT_EQ(memcmp(value.data.ptr(), pData, dataSize),0) << "memory should compare equal";
    // erasing the data
    attributes.erase(pKey);
    EXPECT_EQ(attributes.size(), 0);
    EXPECT_FALSE(attributes.contains(pKey));
    // fetching raw data
    const auto empty = attributes.get(pKey);
    EXPECT_EQ(empty.pKey, nullptr);
    EXPECT_EQ(empty.pDescriptor, nullptr);
    EXPECT_EQ(empty.data.size(), 0);
    EXPECT_NE(empty.data.ptr(), nullptr);
}
