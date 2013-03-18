/*
 * Timeline.h
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace duke {

class IMediaStream;
class IOverlay;

struct Range {
	Range();
	Range(size_t first, size_t last);

	Range& operator+=(size_t frame);

	Range operator+(size_t frame) const;
	bool operator==(const Range&other) const;
	size_t count() const;

	size_t first, last;
	static const Range EMPTY;
};

typedef std::vector<Range> Ranges;

namespace rangeutils {

Range range_union(const Range&a, const Range&b);
bool consecutive(const Range&a, const Range&b);
Range mergeConsecutive(const Range&a, const Range&b);
void mergeConsecutive(Ranges &ranges);

}  // namespace rangeutils

struct Clip {
	size_t frames;
	std::shared_ptr<IMediaStream> pStream;
	std::shared_ptr<IOverlay> pOverlay;
};

typedef std::pair<const IMediaStream*, size_t> MediaFrameReference;

struct Track: public std::map<size_t, Clip> {
	typedef value_type TrackClip;

	void add(size_t frame, Clip&& clip);

	bool contains(const_iterator itr, size_t frame) const;
	bool isClipAt(size_t frame) const;
	const_iterator clipContaining(size_t frame) const;
	const_iterator nextClip(size_t frame) const;
	const_iterator previousClip(size_t frame) const;
	MediaFrameReference getMediaFrameReferenceAt(size_t frame) const;
	Ranges getClipsRange() const;
	Range getRange() const;

	mutable bool disabled = false;
	std::string name;
};

namespace rangeutils {
Range getRange(const Track::TrackClip& trackClip);
}

struct Timeline: public std::vector<Track> {
	Timeline() = default;
	Timeline(std::initializer_list<value_type> initializers) : std::vector<Track>(initializers) {}

	const Track* findTrack(const char* pName) const;

	void populateMediaAt(size_t frame, std::vector<MediaFrameReference> &frames) const;
	Range getRange() const;
	bool empty() const;
};

template<typename C> typename C::const_iterator findLessOrEquals(const C& container, const typename C::key_type& key) {
	const auto end = container.end();
	if (container.empty())
		return end;
	auto bound = container.upper_bound(key);
	return bound == container.begin() ? end : --bound;
}

template<typename C> typename C::const_iterator findLess(const C& container, const typename C::key_type& key) {
	const auto end = container.end();
	if (container.empty())
		return end;
	auto bound = container.lower_bound(key);
	return bound == container.begin() ? end : --bound;
}

}  // namespace duke
