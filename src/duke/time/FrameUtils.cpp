/*
 * FrameUtils.cpp
 *
 *  Created on: Dec 29, 2012
 *      Author: Guillaume Chatelet
 */

#include "FrameUtils.h"

#include <limits>
#include <stdexcept>

FrameRate FrameRate::PAL(25, 1);
FrameRate FrameRate::FILM(24, 1);
FrameRate FrameRate::NTSC(30000, 1001);

std::chrono::microseconds frameToTime(uint32_t frame, const FrameRate framerate) {
	static_assert(std::is_same<decltype(frame),uint32_t>::value,"checking type to prevent overflow");
	static_assert(std::is_same<decltype(framerate.denominator),uint16_t>::value,"checking type to prevent overflow");
	// 32 + 16 + log2(1000) = 58 bits < 63 bits ( 64 - 1 bit sign )
	return frame * framerate.denominator * std::chrono::microseconds(1000) / framerate.numerator;
}

uint32_t timeToFrame(std::chrono::microseconds time, const FrameRate framerate) {
	if (framerate.numerator > 0 && std::chrono::microseconds::max() / framerate.numerator < time)
		throw std::overflow_error("time overflow");
	return time * framerate.numerator / (framerate.denominator * std::chrono::microseconds(1000));
}
