/*
 * Timeline.h
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef TIMELINE_H_
#define TIMELINE_H_

#include <string>
#include <vector>
#include <map>

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

class MediaStream; // forward decl

struct Clip {
	size_t frames;
	MediaStream *pStream;
};

typedef std::pair<const Clip*, size_t> MediaFrameReference;

struct Track {
	std::string name;
	std::map<size_t, Clip> clips;
	void add(size_t frame, Clip&& clip) {
		clips.insert(std::make_pair(frame,std::move(clip)));
	}
	MediaFrameReference clipAt(size_t frame) const;
	Range getRange() const;
};

struct Timeline {
	Timeline() :
			startFrame(0) {
	}
	std::vector<Track> tracks;
	size_t startFrame;
	void populateMediaAt(size_t frame, std::vector<MediaFrameReference> &frames) const;
	Range getRange() const;
};

#endif /* TIMELINE_H_ */
