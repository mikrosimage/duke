#include <gtest/gtest.h>

#include "duke/engine/Timeline.hpp"

using namespace std;
using namespace duke;

TEST(Range, consecutive) {
  EXPECT_FALSE(rangeutils::consecutive(Range(0, 0), Range(0, 0)));
  EXPECT_TRUE(rangeutils::consecutive(Range(0, 0), Range(1, 1)));
  EXPECT_TRUE(rangeutils::consecutive(Range(1, 2), Range(3, 4)));
  EXPECT_EQ(Range(0, 1), rangeutils::mergeConsecutive(Range(0, 0), Range(1, 1)));
  EXPECT_EQ(Range(1, 4), rangeutils::mergeConsecutive(Range(1, 2), Range(3, 4)));
}

TEST(Range, mergeConsecutive) {
  Ranges consecutiveRanges = {Range(0, 0), Range(1, 1), Range(2, 2)};
  rangeutils::mergeConsecutive(consecutiveRanges);
  EXPECT_EQ(1UL, consecutiveRanges.size());
  EXPECT_EQ(Range(0, 2), consecutiveRanges[0]);
}

TEST(Range, mergeNonConsecutive) {
  Ranges consecutiveRanges = {Range(0, 0), Range(2, 2)};
  rangeutils::mergeConsecutive(consecutiveRanges);
  EXPECT_EQ(2UL, consecutiveRanges.size());
  EXPECT_EQ(Range(0, 0), consecutiveRanges[0]);
  EXPECT_EQ(Range(2, 2), consecutiveRanges[1]);
}
