#include <duke/base/SmallBuffer.hpp>

#include <algorithm>
#include <iterator>

#include <gtest/gtest.h>

typedef SmallBuffer<8> SmallData8;

using namespace std;

void checkInvariant(const SmallData8& data, const size_t expected_size) {
  ASSERT_EQ(data.size(), expected_size);
  ASSERT_EQ(data.end(), data.begin() + data.size());
}

TEST(SmallBuffer, Invariants) {
  checkInvariant(SmallData8(0), 0);
  checkInvariant(SmallData8(8), 8);
  checkInvariant(SmallData8(9), 9);
}

void fill(SmallData8& data) {
  uint8_t counter = 0;
  for (auto I = data.begin(), E = data.end(); I != E; ++I, ++counter) *I = counter;
}

SmallData8 getSmallData(size_t size) {
  SmallData8 data(size);
  fill(data);
  return data;
}

SmallData8 getSmallData() { return getSmallData(SmallData8::SMALL_SIZE); }

SmallData8 getExceededSmallData() { return getSmallData(SmallData8::SMALL_SIZE + 1); }

bool ptrIsInsideObject(const SmallData8& object, const void* ptr) {
  return ptr >= reinterpret_cast<const void*>(&object) &&
         ptr <= reinterpret_cast<const void*>(&object + sizeof(object));
}

void checkPtrAllocation(const SmallData8& data, const bool expectedInside) {
  ASSERT_EQ(ptrIsInsideObject(data, data.begin()), expectedInside);
  ASSERT_EQ(ptrIsInsideObject(data, data.end()), expectedInside);
}

TEST(SmallBuffer, SmallIsInsideObject) { checkPtrAllocation(getSmallData(), true); }

TEST(SmallBuffer, BigIsOutsideObject) { checkPtrAllocation(getExceededSmallData(), false); }

bool isFilled(const SmallData8& data) {
  uint8_t counter = 0;
  for (auto I = data.begin(), E = data.end(); I != E; ++I, ++counter)
    if (*I != counter) return false;
  return true;
}

bool equals(const SmallData8& a, const SmallData8& b) {
  return a.size() == b.size() && equal(begin(a), end(a), begin(b));
}

void checkCopy(const SmallData8& data) {
  const auto copy(data);
  ASSERT_TRUE(equals(data, copy));
}

TEST(SmallBuffer, Copy) {
  checkCopy(getSmallData());
  checkCopy(getExceededSmallData());
}

void checkCopyAssign(const SmallData8& data) {
  SmallData8 original;
  ASSERT_TRUE(original.empty());
  original = data;
  ASSERT_FALSE(original.empty());
  ASSERT_TRUE(equals(original, data));
}

TEST(SmallBuffer, Assign) {
  checkCopyAssign(getSmallData());
  checkCopyAssign(getExceededSmallData());
}

void checkMove(const SmallData8& data) {
  auto copy(data);
  ASSERT_TRUE(equals(copy, data));
  ASSERT_FALSE(copy.empty());
  SmallData8 moved(move(copy));
  ASSERT_FALSE(equals(copy, data));
  ASSERT_TRUE(copy.empty());
  ASSERT_TRUE(equals(moved, data));
}

TEST(SmallBuffer, Move) {
  checkMove(getSmallData());
  checkMove(getExceededSmallData());
}

void checkMoveAssign(const SmallData8& data) {
  auto copy(data);
  ASSERT_TRUE(equals(copy, data));
  ASSERT_FALSE(copy.empty());
  SmallData8 moved;
  moved = move(copy);
  ASSERT_FALSE(equals(copy, data));
  ASSERT_TRUE(copy.empty());
  ASSERT_TRUE(equals(moved, data));
}

TEST(SmallBuffer, MoveAssign) {
  checkMoveAssign(getSmallData());
  checkMoveAssign(getExceededSmallData());
}
