/*
 * Timeline.cpp
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#include "Timeline.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

using namespace std;
const Range Range::EMPTY(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::min());

MediaFrameReference Track::clipAt(size_t frame) const {
	if (clips.empty())
		return MediaFrameReference();
	auto pNext = clips.upper_bound(frame);
	if (pNext == clips.begin())
		return MediaFrameReference();
	--pNext;
	const auto frameInClip = frame - pNext->first;
	const auto& clip = pNext->second;
	return frameInClip < clip.frames ? MediaFrameReference(&clip, frameInClip) : MediaFrameReference();
}

Range Track::getRange() const {
	if (clips.empty())
		throw std::runtime_error("no range for empty track");
	const auto pFirst = clips.begin();
	const auto pLast = clips.rbegin();
	return Range(pFirst->first, pLast->first + pLast->second.frames - 1);
}

void Timeline::populateMediaAt(size_t frame, std::vector<MediaFrameReference> &frames) const {
	frames.clear();
	const size_t frameInTimeline = frame - startFrame;
	for (const Track& track : tracks)
		frames.push_back(track.clipAt(frameInTimeline));
}

Range Timeline::getRange() const {
	if (tracks.empty())
		return Range::EMPTY;
	Range result = Range::EMPTY;
	for (const Track& track : tracks) {
		if (track.clips.empty())
			continue;
		const Range trackRange = track.getRange() + startFrame;
		result = result || trackRange;
	}
	return result;
}
