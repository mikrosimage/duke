/*
 * Timeline.cpp
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#include "Timeline.h"
#include <algorithm>

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

void Timeline::populateMediaAt(size_t frame, std::vector<MediaFrameReference> &frames) const {
	frames.clear();
	const size_t frameInTimeline = frame - startFrame;
	for (const Track& track : tracks)
		frames.push_back(track.clipAt(frameInTimeline));
}
