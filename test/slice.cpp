#include <duke/base/Slice.hpp>

#include <gtest/gtest.h>

TEST(Slice, Empty) {
  Slice<char> empty;
  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(0, empty.size());
  EXPECT_EQ(nullptr, empty.begin());
  EXPECT_EQ(nullptr, empty.end());
}

TEST(Slice, Allocated) {
  char buffer[2];
  Slice<char> slice(std::begin(buffer), std::end(buffer));
  EXPECT_FALSE(slice.empty());
  EXPECT_EQ(2, slice.size());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer), slice.begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 2, slice.end());
}

TEST(Slice, AllocatedFromArray) {
  char buffer[2];
  Slice<char> slice(buffer);
  EXPECT_FALSE(slice.empty());
  EXPECT_EQ(2, slice.size());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer), slice.begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 2, slice.end());
}

TEST(Slice, PopFront) {
  char buffer[5];
  Slice<char> slice(buffer);
  EXPECT_EQ(5, slice.pop_front(0).size());
  EXPECT_EQ(5, slice.pop_front(6).size());
  EXPECT_EQ(4, slice.pop_front(1).size());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 1, slice.pop_front(1).begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 5, slice.pop_front(1).end());
  EXPECT_EQ(0, slice.pop_front(5).size());
  EXPECT_TRUE(slice.pop_front(5).empty());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 5, slice.pop_front(5).begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 5, slice.pop_front(5).end());
}

TEST(Slice, PopBack) {
  char buffer[5];
  Slice<char> slice(buffer);
  EXPECT_EQ(5, slice.pop_back(0).size());
  EXPECT_EQ(5, slice.pop_back(6).size());
  EXPECT_EQ(4, slice.pop_back(1).size());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer), slice.pop_back(1).begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 4, slice.pop_back(1).end());
  EXPECT_EQ(0, slice.pop_back(5).size());
  EXPECT_TRUE(slice.pop_back(5).empty());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer), slice.pop_back(5).begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer), slice.pop_back(5).end());
}

TEST(Slice, Resize) {
  char buffer[5];
  Slice<char> slice(buffer);
  EXPECT_EQ(5, slice.size());
  EXPECT_EQ(5, slice.resize(6).size());
  EXPECT_EQ(5, slice.resize(5).size());
  EXPECT_EQ(1, slice.resize(1).size());
  EXPECT_EQ(1, slice.resize(1).resize(2).size());
  EXPECT_EQ(0, slice.resize(0).size());
  EXPECT_TRUE(slice.resize(0).empty());
}

TEST(Slice, FrontBack) {
  char buffer[1] = {'a'};
  Slice<char> slice(buffer);
  EXPECT_EQ('a', slice.front());
  EXPECT_EQ('a', slice.back());
  slice.front() = 'b';
  EXPECT_EQ('b', slice.front());
  EXPECT_EQ('b', slice.back());
  slice.back() = 'c';
  EXPECT_EQ('c', slice.front());
  EXPECT_EQ('c', slice.back());
}
