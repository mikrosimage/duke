#include <gtest/gtest.h>

#include <duke/attributes/AttributeKeys.hpp>
#include <duke/attributes/Attributes.hpp>
#include <glm/glm.hpp>

using namespace std;

struct PlainOldData {
    double a;
    int b;
    bool operator==(const PlainOldData& other) const {
        return a == other.a && b == other.b;
    }
};

enum Enum { ONE, TWO };

namespace attribute {

DECLARE_ATTRIBUTE(std::string, StringAttribute, "");
DECLARE_ATTRIBUTE(const char*, CStringAttribute, nullptr);
DECLARE_ATTRIBUTE(uint64_t, Uint64Attribute, 0);
DECLARE_ATTRIBUTE(PlainOldData, PODAttribute, {});
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
void testSuite(const typename T::value_type& defaultValue, const typename T::value_type& value) {
    Attributes attributes;
    EXPECT_FALSE(attributes.contains<T>());
    EXPECT_EQ(attributes.size(), 0);
    EXPECT_EQ(attributes.getOrDefault<T>(), T::default_value());
    EXPECT_EQ(attributes.getWithDefault<T>(defaultValue), defaultValue);
    attributes.set<T>(value);
    EXPECT_EQ(attributes.size(), 1);
    attributes.set<T>(value);
    EXPECT_EQ(attributes.size(), 1);
    EXPECT_TRUE(attributes.contains<T>());
    compare<typename T::value_type>(attributes.getOrDie<T>(), value);
    compare<typename T::value_type>(attributes.getOrDefault<T>(), value);
    compare<typename T::value_type>(attributes.getWithDefault<T>(defaultValue), value);
    attributes.erase<T>();
    EXPECT_EQ(attributes.size(), 0);
    EXPECT_FALSE(attributes.contains<T>());
}

TEST(Attributes, string) {
    testSuite<StringAttribute>("default", "value");
}

TEST(Attributes, cstring) {
    testSuite<CStringAttribute>("default", "value");
    Attributes attributes;
}

TEST(Attributes, integral) {
    testSuite<Uint64Attribute>(0ULL, 123ULL);
}

TEST(Attributes, POD) {
    testSuite<PODAttribute>( { 0.0, 0 }, { 1.0, 1 });
}

TEST(Attributes, enum) {
    testSuite<EnumAttribute>(Enum::ONE, Enum::TWO);
}
