#include <gtest/gtest.h>

#include <duke/animation/Animation.h>

using namespace duke;

TEST(Timeinterpolation,noRepeat) {
	TimeCycleEvaluator anim(100);
	anim.repeatCount = 0;
	anim.repeatMode = RepeatMode::RESTART;
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(-1));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(0));
	EXPECT_DOUBLE_EQ(.25, anim.getInterpolatedTime(25));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(100));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(125));
	anim.repeatMode = RepeatMode::REVERSE;
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(-1));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(0));
	EXPECT_DOUBLE_EQ(.25, anim.getInterpolatedTime(25));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(100));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(125));
}

TEST(Timeinterpolation,oneRepeat) {
	TimeCycleEvaluator anim(100);
	anim.repeatCount = 1;
	anim.repeatMode = RepeatMode::RESTART;
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(-1));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(0));
	EXPECT_DOUBLE_EQ(.25, anim.getInterpolatedTime(25));
	EXPECT_DOUBLE_EQ(.99, anim.getInterpolatedTime(99));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(100));
	EXPECT_DOUBLE_EQ(.25, anim.getInterpolatedTime(125));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(200));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(201));
	anim.repeatMode = RepeatMode::REVERSE;
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(-1));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(0));
	EXPECT_DOUBLE_EQ(.25, anim.getInterpolatedTime(25));
	EXPECT_DOUBLE_EQ(.99, anim.getInterpolatedTime(99));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(100));
	EXPECT_DOUBLE_EQ(.75, anim.getInterpolatedTime(125));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(200));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(201));
}

TEST(Timeinterpolation,infiniteRepeat) {
	TimeCycleEvaluator anim(100);
	anim.repeatCount = RepeatCount::INFINITE;
	anim.repeatMode = RepeatMode::RESTART;
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(-1));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(0));
	EXPECT_DOUBLE_EQ(0.01, anim.getInterpolatedTime(1));
	EXPECT_DOUBLE_EQ(.25, anim.getInterpolatedTime(25));
	EXPECT_DOUBLE_EQ(.99, anim.getInterpolatedTime(99));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(100));
	EXPECT_DOUBLE_EQ(.25, anim.getInterpolatedTime(125));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(200));
	EXPECT_DOUBLE_EQ(.01, anim.getInterpolatedTime(201));
	anim.repeatMode = RepeatMode::REVERSE;
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(-1));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(0));
	EXPECT_DOUBLE_EQ(.25, anim.getInterpolatedTime(25));
	EXPECT_DOUBLE_EQ(.99, anim.getInterpolatedTime(99));
	EXPECT_DOUBLE_EQ(1, anim.getInterpolatedTime(100));
	EXPECT_DOUBLE_EQ(.75, anim.getInterpolatedTime(125));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(200));
	EXPECT_DOUBLE_EQ(0.25, anim.getInterpolatedTime(225));
}

TEST(Timeinterpolation,startDelay) {
	TimeCycleEvaluator anim(100);
	anim.repeatCount = 0;
	anim.repeatMode = RepeatMode::RESTART;
	anim.startTime = 50;
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(0));
	EXPECT_DOUBLE_EQ(0, anim.getInterpolatedTime(50));
	EXPECT_DOUBLE_EQ(0.5, anim.getInterpolatedTime(100));
}
