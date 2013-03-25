#include <gtest/gtest.h>

#include <duke/animation/Animation.h>

using namespace duke;

TEST(Timeinterpolation,noRepeat) {
	AnimationData data;
	data.duration = 100;
	data.repeatCount = 0;
	data.repeatMode = RepeatMode::RESTART;
	EXPECT_DOUBLE_EQ(0, getCycleValue(data, -1));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data, 0));
	EXPECT_DOUBLE_EQ(.25, getCycleValue(data, 25));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data, 100));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data, 125));
	data.repeatMode = RepeatMode::REVERSE;
	EXPECT_DOUBLE_EQ(0, getCycleValue(data, -1));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data, 0));
	EXPECT_DOUBLE_EQ(.25, getCycleValue(data, 25));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data, 100));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data, 125));
}

TEST(Timeinterpolation,oneRepeat) {
	AnimationData data;
	data.duration = 100;
	data.repeatCount = 1;
	data.repeatMode = RepeatMode::RESTART;
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,-1));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,0));
	EXPECT_DOUBLE_EQ(.25, getCycleValue(data,25));
	EXPECT_DOUBLE_EQ(.99, getCycleValue(data,99));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,100));
	EXPECT_DOUBLE_EQ(.25, getCycleValue(data,125));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data,200));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data,201));
	data.repeatMode = RepeatMode::REVERSE;
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,-1));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,0));
	EXPECT_DOUBLE_EQ(.25, getCycleValue(data,25));
	EXPECT_DOUBLE_EQ(.99, getCycleValue(data,99));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data,100));
	EXPECT_DOUBLE_EQ(.75, getCycleValue(data,125));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data,200));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data,201));
}

TEST(Timeinterpolation,infiniteRepeat) {
	AnimationData data;
	data.duration = 100;
	data.repeatCount = RepeatCount::INFINITE;
	data.repeatMode = RepeatMode::RESTART;
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,-1));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,0));
	EXPECT_DOUBLE_EQ(0.01, getCycleValue(data,1));
	EXPECT_DOUBLE_EQ(.25, getCycleValue(data,25));
	EXPECT_DOUBLE_EQ(.99, getCycleValue(data,99));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,100));
	EXPECT_DOUBLE_EQ(.25, getCycleValue(data,125));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,200));
	EXPECT_DOUBLE_EQ(.01, getCycleValue(data,201));
	data.repeatMode = RepeatMode::REVERSE;
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,-1));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,0));
	EXPECT_DOUBLE_EQ(.25, getCycleValue(data,25));
	EXPECT_DOUBLE_EQ(.99, getCycleValue(data,99));
	EXPECT_DOUBLE_EQ(1, getCycleValue(data,100));
	EXPECT_DOUBLE_EQ(.75, getCycleValue(data,125));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,200));
	EXPECT_DOUBLE_EQ(0.25, getCycleValue(data,225));
}

TEST(Timeinterpolation,startDelay) {
	AnimationData data;
	data.duration = 100;
	data.repeatCount = 0;
	data.repeatMode = RepeatMode::RESTART;
	data.startTime = 50;
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,0));
	EXPECT_DOUBLE_EQ(0, getCycleValue(data,50));
	EXPECT_DOUBLE_EQ(0.5, getCycleValue(data,100));
}
