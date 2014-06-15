#include "duke/base/Slice.hpp"

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
  EXPECT_EQ(5, pop_front(slice, 0).size());
  EXPECT_EQ(4, pop_front(slice, 1).size());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 1, pop_front(slice, 1).begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 5, pop_front(slice, 1).end());
  EXPECT_EQ(0, pop_front(slice, 5).size());
  EXPECT_TRUE(pop_front(slice, 5).empty());
}

TEST(Slice, PopBack) {
  char buffer[5];
  Slice<char> slice(buffer);
  EXPECT_EQ(5, pop_back(slice, 0).size());
  EXPECT_EQ(4, pop_back(slice, 1).size());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer), pop_back(slice, 1).begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 4, pop_back(slice, 1).end());
  EXPECT_EQ(0, pop_back(slice, 5).size());
  EXPECT_TRUE(pop_back(slice, 5).empty());
}

TEST(Slice, KeepFront) {
  char buffer[5];
  Slice<char> slice(buffer);
  EXPECT_EQ(0, keep_front(slice, 0).size());
  EXPECT_EQ(1, keep_front(slice, 1).size());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer), keep_front(slice, 1).begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 1, keep_front(slice, 1).end());
}

TEST(Slice, KeepBack) {
  char buffer[5];
  Slice<char> slice(buffer);
  EXPECT_EQ(0, keep_back(slice, 0).size());
  EXPECT_EQ(1, keep_back(slice, 1).size());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 4, keep_back(slice, 1).begin());
  EXPECT_EQ(reinterpret_cast<char*>(&buffer) + 5, keep_back(slice, 1).end());
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
