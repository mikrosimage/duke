#include <gtest/gtest.h>

#include <duke/imageio/Attribute.h>
#include <duke/imageio/Attributes.h>
#include <glm/glm.hpp>

using namespace std;

TEST(Attribute,defaultCtor) {
	Attribute attr;
	EXPECT_EQ(nullptr, attr.data());
	EXPECT_TRUE(attr.name().empty());
	EXPECT_EQ(0, attr.size());
}
TEST(Attribute,intValue) {
	Attribute attr("v", 1);
	EXPECT_EQ("v", attr.name());
	EXPECT_EQ(1, *(int*)attr.data());
	EXPECT_EQ(1, attr.getScalar<int>());
	EXPECT_EQ(0, attr.size());
}
TEST(Attribute,doubleValue) {
	Attribute attr("v", 1.);
	EXPECT_EQ("v", attr.name());
	EXPECT_EQ(1., *(double*)attr.data());
	EXPECT_EQ(1., attr.getScalar<double>());
	EXPECT_EQ(0, attr.size());
}
TEST(Attribute,stringValue) {
	Attribute attr("v", "hello");
	EXPECT_EQ("v", attr.name());
	EXPECT_STREQ("hello", (const char*)attr.data());
	EXPECT_STREQ("hello", attr.getString());
	EXPECT_EQ(5, attr.size());
}
TEST(Attribute,vectorValue) {
	const std::vector<float> floats = { 1.f, 2.f };
	Attribute attr("v", floats);
	EXPECT_EQ("v", attr.name());
	EXPECT_EQ(1.f, *(const float*)attr.data());
	EXPECT_EQ(1.f, *attr.getVector<float>().begin());
	EXPECT_EQ(2, attr.size());
	const auto v = attr.getVector<float>();
	const std::vector<float> floatsback(v.begin(), v.end());
	EXPECT_EQ(floatsback, floats);
}
TEST(Attribute,badDereferencing) {
	EXPECT_THROW(Attribute().getScalar<float>(), std::bad_cast);
	EXPECT_THROW(Attribute("v", 1).getScalar<float>(), std::bad_cast);
	EXPECT_THROW(Attribute("", 1).getString(), std::bad_cast);
	EXPECT_THROW(Attribute("", 1).getVector<int>(), std::bad_cast);
}
TEST(Attribute,move) {
	Attribute original("name", 100);
	Attribute copy(std::move(original));
	EXPECT_STREQ("name", copy.name().c_str());
	EXPECT_EQ(100, original.getScalar<int>());
}
TEST(Attribute,copy) {
	Attribute original("name", 100);
	Attribute copy(original);
	EXPECT_EQ(copy.name(), original.name());
	EXPECT_EQ(copy.getScalar<int>(), original.getScalar<int>());
}
TEST(Attribute,assignement) {
	Attribute original("name", 100);
	Attribute copy("something", 1.f);
	copy = original;
	EXPECT_EQ(copy.name(), original.name());
	EXPECT_EQ(copy.getScalar<int>(), original.getScalar<int>());
}
TEST(Attributes,find) {
	Attributes attributes;
	attributes.emplace_back("name", 100);
	EXPECT_TRUE(attributes.find("name", PrimitiveType::INT));
	EXPECT_TRUE(attributes.find<int>("name"));
	EXPECT_FALSE(attributes.find("name", PrimitiveType::FLOAT));
	EXPECT_FALSE(attributes.find("name_", PrimitiveType::INT));
	EXPECT_THROW(attributes.findVector<int>("name"),std::runtime_error);
}
TEST(Attributes,findVector) {
	Attributes attributes;
	attributes.emplace_back("name", std::vector<float>({1,2,3}));
	attributes.findVector<float>("name");
}
