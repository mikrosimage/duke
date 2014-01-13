#include <gtest/gtest.h>

#include <duke/base/SmallVector.hpp>

#include <vector>

typedef std::vector<char> Buffer;
typedef SmallVector<char> CharVector;

TEST(SmallVector,DefaultCtor) {
    CharVector vector;
    EXPECT_EQ(vector.size(), 0);
    EXPECT_EQ(vector.isAllocated(), false);
}

TEST(SmallVector,NoAllocation) {
    const size_t allocSize = CharVector::kSmallSize;
    const Buffer buffer(allocSize, 'X');
    CharVector vector(buffer.data(), buffer.size());
    EXPECT_EQ(vector.size(), allocSize);
    EXPECT_EQ(vector.isAllocated(), false);
    EXPECT_TRUE(std::equal(vector.begin(), vector.end(), buffer.begin()));
}

TEST(SmallVector,Allocation) {
    const size_t allocSize = CharVector::kSmallSize + 1;
    const Buffer buffer(allocSize, 'X');
    CharVector vector(buffer.data(), buffer.size());
    EXPECT_EQ(vector.size(), allocSize);
    EXPECT_EQ(vector.isAllocated(), true);
    EXPECT_TRUE(std::equal(vector.begin(), vector.end(), buffer.begin()));
}

TEST(SmallVector,InitilizationList) {
    CharVector vector { '1' };
    EXPECT_EQ(vector.size(), 1);
    EXPECT_EQ(vector.isAllocated(), false);
    EXPECT_EQ(*vector.data(), '1');
}

TEST(SmallVector,Copy) {
    const CharVector vector { '1' };
    EXPECT_EQ(vector.size(), 1);
    EXPECT_EQ(vector.isAllocated(), false);
    EXPECT_EQ(vector.data()[0], '1');
    CharVector copy(vector);
    EXPECT_EQ(copy.size(), 1);
    EXPECT_EQ(copy.isAllocated(), false);
    EXPECT_EQ(copy.data()[0], '1');
}

TEST(SmallVector,Assignement) {
    CharVector vector { '1' };
    EXPECT_EQ(vector.size(), 1);
    EXPECT_EQ(vector.isAllocated(), false);
    EXPECT_EQ(vector.data()[0], '1');
    vector = CharVector { '1', '2' };
    EXPECT_EQ(vector.size(), 2);
    EXPECT_EQ(vector.isAllocated(), false);
    EXPECT_EQ(vector.data()[0], '1');
    EXPECT_EQ(vector.data()[1], '2');
}

TEST(SmallVector,Equals) {
    const CharVector a { '1' };
    const CharVector b { '1' };
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(SmallVector,NotEquals) {
    const CharVector a { '1' };
    const CharVector b { '1', '2' };
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(SmallVector, Less) {
    const CharVector a { '1' };
    EXPECT_FALSE(a < a);

    const CharVector b { '2' };
    EXPECT_TRUE(a < b);

    const CharVector c { '1', '2' };
    EXPECT_TRUE(a < c);
    EXPECT_TRUE(b < c);

    const CharVector d { '1', '1' };
    EXPECT_TRUE(d < c);
}

TEST(SmallVector, Move) {
    // TODO
}
