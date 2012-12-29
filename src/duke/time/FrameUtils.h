#ifndef FRAMEUTILS_H_
#define FRAMEUTILS_H_

#include <chrono>

struct FrameRate {
	uint16_t numerator;
	uint16_t denominator;
	FrameRate(uint16_t num, uint16_t den) :
			numerator(num), denominator(den) {
	}

	static FrameRate PAL, FILM, NTSC;
};

std::chrono::microseconds frameToTime(uint32_t frame, const FrameRate framerate);
uint32_t timeToFrame(std::chrono::microseconds time, const FrameRate framerate);

#endif /* FRAMEUTILS_H_ */
