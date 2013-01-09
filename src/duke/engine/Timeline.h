/*
 * Timeline.h
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef TIMELINE_H_
#define TIMELINE_H_

#include <duke/engine/renderers/IRenderer.h>

#include <string>
#include <vector>
#include <map>
#include <memory>

struct Range {
	Range() :
			first(0), last(0) {
	}
	Range(size_t first, size_t last) :
			first(first), last(last) {
	}
	bool operator==(const Range&other) const {
		return first == other.first && last == other.last;
	}
	Range operator+(size_t frame) const {
		return Range(first + frame, last + frame);
	}
	Range& operator+=(size_t frame) {
		first += frame;
		last += frame;
		return *this;
	}
	Range operator||(const Range&other) const {
		return Range(first < other.first ? first : other.first, last > other.last ? last : other.last);
	}
	size_t first, last;
	static const Range EMPTY;
};

namespace duke {
class MediaStream;
}  // namespace duke

struct Clip {
	size_t frames;
	std::shared_ptr<duke::MediaStream> pStream;
};

typedef std::pair<const Clip*, size_t> MediaFrameReference;

struct Track: public std::map<size_t, Clip> {
	bool disabled = false;
	std::string name;
	void add(size_t frame, Clip&& clip) {
		insert(std::make_pair(frame,std::move(clip)));
	}
	const_iterator clipContaining(size_t frame) const;
	const_iterator nextClip(size_t frame) const;
	const_iterator previousClip(size_t frame) const;
	MediaFrameReference getClipFrame(size_t frame) const;
	Range getRange() const;
	bool contains(const_iterator itr, size_t frame) const;
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

struct Timeline: public std::vector<Track> {
	Timeline() = default;
	Timeline(std::initializer_list<value_type> initializers) : std::vector<Track>(initializers) {}
	void populateMediaAt(size_t frame, std::vector<MediaFrameReference> &frames) const;
	Range getRange() const;
	bool empty() const;
};

#endif /* TIMELINE_H_ */
