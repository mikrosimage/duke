#include <duke/attributes/AttributesView.hpp>

#include <gtest/gtest.h>

#include <vector>
#include <string>

using namespace attribute;
using namespace std;

vector<string> getKeys(const AttributesView& view) {
  vector<string> order;
  for (const Attribute* pAttribute : view) order.push_back(pAttribute->name);
  return order;
}

TEST(AttributesView, Basic) {
  const Attributes first = {Attribute("a", 0)};
  const Attributes second = {Attribute("b", 1)};
  AttributesView view;
  EXPECT_TRUE(view.empty());
  EXPECT_EQ(0, view.size());
  view.merge(second);
  EXPECT_FALSE(view.empty());
  EXPECT_EQ(1, view.size());
  view.merge(first);
  EXPECT_FALSE(view.empty());
  EXPECT_EQ(2, view.size());
  EXPECT_EQ((vector<string>{"b", "a"}), getKeys(view));
  view.sort();
  EXPECT_EQ((vector<string>{"a", "b"}), getKeys(view));
}
