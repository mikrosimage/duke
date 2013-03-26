#include "FrameUtils.hpp"

#include <limits>
#include <ratio>
#include <stdexcept>

using namespace std;

const FrameDuration FrameDuration::FILM(1, 24);
const FrameDuration FrameDuration::PAL(1, 25);
const FrameDuration FrameDuration::NTSC(1001, 30000);

Time frameToTime(const uint32_t frame, const FrameDuration &period) {
	return FrameDuration(period) *= frame;
}

FrameIndex timeToFrame(Time time, const FrameDuration &period) {
	return time /= period;
}
