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
#include <cassert>

using namespace std;

namespace duke {

const Range Range::EMPTY(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::min());

Range::Range() :
		first(0), last(0) {
}
Range::Range(size_t first, size_t last) :
		first(first), last(last) {
}
bool Range::operator==(const Range&other) const {
	return first == other.first && last == other.last;
}
Range Range::operator+(size_t frame) const {
	return Range(first + frame, last + frame);
}
Range& Range::operator+=(size_t frame) {
	first += frame;
	last += frame;
	return *this;
}
size_t Range::count() const {
	return first <= last ? last - first + 1 : first - last + 1;
}

namespace rangeutils {

Range range_union(const Range&a, const Range&b) {
	return Range(a.first < b.first ? a.first : b.first, a.last > b.last ? a.last : b.last);
}
bool consecutive(const Range&a, const Range&b) {
	return a.last + 1 == b.first;
}
Range mergeConsecutive(const Range&a, const Range&b) {
	assert(consecutive(a, b));
	return Range(a.first, b.last);
}
void mergeConsecutive(Ranges &ranges) {
	if (ranges.size() <= 1)
		return;
	auto mergeItr = ranges.begin();
	auto testingItr = mergeItr + 1;
	for (; testingItr != ranges.end(); ++testingItr) {
		Range &mergeRange = *mergeItr;
		const Range &testingRange = *testingItr;
		if (consecutive(mergeRange, testingRange))
			mergeRange = mergeConsecutive(mergeRange, testingRange);
		else
			++mergeItr;
	}
	++mergeItr;
	ranges.erase(mergeItr, ranges.end());
}
Range getRange(const Track::TrackClip& trackClip) {
	return Range(trackClip.first, trackClip.first + trackClip.second.frames - 1);
}

}  // namespace rangeutils

MediaFrameReference Track::getMediaFrameReferenceAt(size_t frame) const {
	auto pFound = clipContaining(frame);
	if (pFound != end() && pFound->second.pStream)
		return MediaFrameReference(pFound->second.pStream.get(), frame - pFound->first);
	return MediaFrameReference();
}

void Track::add(size_t frame, Clip&& clip) {
	if(clip.frames==0)
	throw std::logic_error("Can't add a clip with zero frame");
	insert(std::make_pair(frame,std::move(clip)));
}

Track::const_iterator Track::clipContaining(size_t frame) const {
	auto pFound = findLessOrEquals(*this, frame);
	return contains(pFound, frame) ? pFound : end();
}

bool Track::isClipAt(size_t frame) const {
	return clipContaining(frame) != end();
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
	const auto from = *begin();
	const auto to = *rbegin();
	return Range(from.first, to.first + to.second.frames - 1);
}

Ranges Track::getClipsRange() const {
	Ranges ranges;
	for (const TrackClip& pair : *this)
		ranges.push_back(rangeutils::getRange(pair));
	return ranges;
}

void Timeline::populateMediaAt(size_t frame, std::vector<MediaFrameReference> &frames) const {
	frames.clear();
	for (const Track& track : *this)
		frames.push_back(track.getMediaFrameReferenceAt(frame));
}

Range Timeline::getRange() const {
	if (empty())
		return Range::EMPTY;
	Range result = Range::EMPTY;
	for (const Track& track : *this) {
		if (track.empty())
			continue;
		const Range trackRange = track.getRange();
		result = rangeutils::range_union(result, trackRange);
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

const Track* Timeline::findTrack(const char* pName) const {
	auto pFound = find_if(this->begin(), this->end(), [=](const Track &track) ->bool {
		return track.name==pName;
	});
	if (pFound == end())
		return nullptr;
	return &*pFound;
}

}  // namespace duke
