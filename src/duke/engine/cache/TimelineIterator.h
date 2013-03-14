/*
 * TimelineIterator.h
 *
 *  Created on: Feb 5, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TIMELINEITERATOR_H_
#define TIMELINEITERATOR_H_

#include <duke/engine/Timeline.h>

namespace duke {

Ranges getMediaRanges(const Timeline &timeline);
bool contains(const Ranges &range, size_t frame);

struct TrackMediaFrameIterator {
	TrackMediaFrameIterator() = default;
	TrackMediaFrameIterator(TrackMediaFrameIterator&&) = default;
	TrackMediaFrameIterator(const Timeline * pTimeline, size_t currentFrame);

	void reset(const Timeline * pTimeline, size_t currentFrame);

	void clear();
	MediaFrameReference next();
	bool empty() const;
private:
	std::vector<MediaFrameReference> m_References;
};

enum class IterationMode
	: unsigned char {
		FORWARD, BACKWARD, PINGPONG
};

struct FrameIterator {
	FrameIterator(const Ranges *pMediaRanges, size_t initialFrame, IterationMode mode = IterationMode::FORWARD);

	void clear();
	size_t next();
	bool empty() const;
private:
	size_t findNext(size_t) const;
	size_t findPrevious(size_t) const;

	const Ranges *m_pMediaRanges;
	IterationMode m_Mode;
	size_t m_Forward, m_Backward, m_FramesToGo, m_SpanCount;
	bool m_bForward;
};

struct TimelineIterator {
	TimelineIterator();
	TimelineIterator(const Timeline * pTimeline, const Ranges *pMediaRanges, size_t currentFrame);
	void clear();
	MediaFrameReference next();
	bool empty();
	size_t getCurrentFrame() const;
private:
	void regularizeCurrentFrame();
	void stepForward();
	void stepUntilValidOrExhausted();
	bool valid() const;
	bool finished() const;
	const Track& getCurrentTrack() const;

	const Timeline * m_pTimeline;
	const Ranges *m_pMediaRanges;
	size_t m_CurrentFrame;
	size_t m_CurrentTrackIndex;
	size_t m_EndFrame;
};

} /* namespace duke */
#endif /* TIMELINEITERATOR_H_ */
