/*
 * FrameUtils.cpp
 *
 *  Created on: Dec 29, 2012
 *      Author: Guillaume Chatelet
 */

#include "FrameUtils.h"

#include <limits>
#include <ratio>
#include <stdexcept>

using namespace std;

const PlaybackPeriod PlaybackPeriod::FILM(1, 24);
const PlaybackPeriod PlaybackPeriod::PAL(1, 25);
const PlaybackPeriod PlaybackPeriod::NTSC(1001, 30000);

Time frameToTime(const uint32_t frame, const PlaybackPeriod &period) {
	return PlaybackPeriod(period) *= frame;
}

Frame timeToFrame(Time time, const PlaybackPeriod &period) {
	return time /= period;
}
