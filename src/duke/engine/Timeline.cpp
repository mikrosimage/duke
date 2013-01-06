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

MediaFrameReference Track::getClipFrame(size_t frame) const {
	auto pFound = clipContaining(frame);
	if (pFound != end())
		return MediaFrameReference(&pFound->second, frame - pFound->first);
	return MediaFrameReference();
}

Track::const_iterator Track::clipContaining(size_t frame) const {
	auto pFound = findLessOrEquals(*this, frame);
	return contains(pFound, frame) ? pFound : end();
}

Track::const_iterator Track::nextClip(size_t frame) const {
	return upper_bound(frame);
}

Track::const_iterator Track::previousClip(size_t frame) const {
	auto previous = findLess(*this, frame);
	if (previous == end())
		return end();
	if (!contains(previous, frame))
		return previous;
	if (previous == begin())
		return end();
	return --previous;
}

bool Track::contains(const_iterator itr, size_t frame) const {
	return itr != end() && frame - itr->first < itr->second.frames;
}

Range Track::getRange() const {
	if (empty())
		throw std::runtime_error("no range for empty track");
	const auto pFirst = begin();
	const auto pLast = rbegin();
	return Range(pFirst->first, pLast->first + pLast->second.frames - 1);
}

void Timeline::populateMediaAt(size_t frame, std::vector<MediaFrameReference> &frames) const {
	frames.clear();
	for (const Track& track : *this)
		frames.push_back(track.getClipFrame(frame));
}

Range Timeline::getRange() const {
	if (empty())
		return Range::EMPTY;
	Range result = Range::EMPTY;
	for (const Track& track : *this) {
		if (track.empty())
			continue;
		const Range trackRange = track.getRange();
		result = result || trackRange;
	}
	return result;
}

bool Timeline::empty() const {
	if (std::vector<Track>::empty())
		return true;
	for (const Track& track : *this)
		if (!track.empty())
			return false;
	return true;
}
