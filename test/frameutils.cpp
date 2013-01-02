#include <gtest/gtest.h>

#include <duke/time/FrameUtils.h>
#include <chrono>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

TEST(FrameUtils,frameToTime) {
	EXPECT_EQ( Time(0), frameToTime(0,PlaybackPeriod::PAL));
	EXPECT_EQ( Time(1), frameToTime(25,PlaybackPeriod::PAL));
	EXPECT_EQ( Time(1), frameToTime(24,PlaybackPeriod::FILM));
	EXPECT_EQ( Time(1001,1000), frameToTime(30,PlaybackPeriod::NTSC));
}

TEST(FrameUtils,timeToFrame) {
	EXPECT_EQ( Frame(0), timeToFrame(Time(0),PlaybackPeriod::PAL));
	EXPECT_EQ( Frame(25), timeToFrame(Time(1),PlaybackPeriod::PAL));
	EXPECT_EQ( Frame(24), timeToFrame(Time(1),PlaybackPeriod::FILM));
	EXPECT_EQ( Frame(30), timeToFrame(Time(1001,1000),PlaybackPeriod::NTSC));
}

TEST(FrameUtils,timeToMicroseconds) {
	EXPECT_EQ( microseconds(666667), Time(2,3).asMicroseconds());
	EXPECT_EQ( microseconds(1), Time(1,1000000).asMicroseconds());
	EXPECT_EQ( seconds(3600), Time(3600).asMicroseconds());
}

void testBackAndForthCalculations(const PlaybackPeriod framerate) {
	for (auto i = 0; i < 100000; ++i) {
		const auto frame = rand();
		ASSERT_EQ( Frame(frame), timeToFrame(frameToTime(frame,framerate),framerate));
	}
}

TEST(FrameUtils,domain) {
	testBackAndForthCalculations(PlaybackPeriod::PAL);
	testBackAndForthCalculations(PlaybackPeriod::NTSC);
	testBackAndForthCalculations(PlaybackPeriod::FILM);
}
