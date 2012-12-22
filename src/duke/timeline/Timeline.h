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
	size_t first, last;
};

class MediaStream {
public:
	enum Type {
		AUDIO, VIDEO
	} m_Type;
	virtual ~MediaStream() {
	}
};

struct Clip {
	size_t frames;
	MediaStream *pStream;
};

typedef std::pair<const Clip*, size_t> MediaFrameReference;

struct Track {
	std::string name;
	std::map<size_t, Clip> clips;
	MediaFrameReference clipAt(size_t frame) const;
};

struct Timeline {
	std::vector<Track> tracks;
	size_t startFrame;
	void populateMediaAt(size_t frame, std::vector<MediaFrameReference> &frames) const;
};

#endif /* TIMELINE_H_ */
