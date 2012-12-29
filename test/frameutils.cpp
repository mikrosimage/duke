#include <gtest/gtest.h>

#include <duke/time/FrameUtils.h>
#include <chrono>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

TEST(FrameUtils,frameToTime) {
	EXPECT_EQ( microseconds(0), frameToTime(0,FrameRate::PAL));
	EXPECT_EQ( microseconds(1000), frameToTime(25,FrameRate::PAL));
	EXPECT_EQ( microseconds(1000), frameToTime(24,FrameRate::FILM));
	EXPECT_EQ( microseconds(1001), frameToTime(30,FrameRate::NTSC));
}

TEST(FrameUtils,timeToFrame) {
	EXPECT_EQ( 0, timeToFrame(microseconds(0),FrameRate::PAL));
	EXPECT_EQ( 25, timeToFrame(microseconds(1000),FrameRate::PAL));
	EXPECT_EQ( 24, timeToFrame(microseconds(1000),FrameRate::FILM));
	EXPECT_EQ( 30, timeToFrame(microseconds(1001),FrameRate::NTSC));
}

TEST(FrameUtils,overflow) {
	const auto maxuint16 = std::numeric_limits<uint16_t>::max();
	const FrameRate maxFrameRate(maxuint16, maxuint16);
	const auto maxMicroSec = std::chrono::microseconds::max();
	EXPECT_THROW(timeToFrame(maxMicroSec, maxFrameRate), std::overflow_error);
}
