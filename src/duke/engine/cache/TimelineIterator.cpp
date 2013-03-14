/*
 * TimelineIterator.cpp
 *
 *  Created on: Feb 5, 2013
 *      Author: Guillaume Chatelet
 */

#include "TimelineIterator.h"
#include <queue>
#include <algorithm>
#include <cassert>

namespace duke {

const static MediaFrameReference EMPTY;

struct RangeComparator: std::binary_function<Range, Range, bool> {
	bool operator()(const Range& x, const Range& y) const {
		return x.first > y.first;
	}
};

Ranges getMediaRanges(const Timeline &timeline) {
	using namespace rangeutils;
	std::priority_queue<Range, Ranges, RangeComparator> mediaRanges;
	for (const Track &track : timeline)
		for (const auto &pair : track)
			if (pair.second.pStream)
				mediaRanges.push(getRange(pair));
	if (mediaRanges.empty())
		return Ranges();
	Ranges merged;
	merged.push_back(mediaRanges.top());
	mediaRanges.pop();
	for (; !mediaRanges.empty(); mediaRanges.pop()) {
		Range &lastMerged = merged.back();
		const Range &current = mediaRanges.top();
		if (lastMerged.last + 1 >= current.first) {
			lastMerged.last = current.last;
		} else {
			merged.push_back(current);
		}
	}
	return merged;
}

bool contains(const Ranges &range, size_t frame) {
	return std::find_if(begin(range), end(range), [=](const Range &range) {
		return range.first<=frame && range.last>=frame;
	}) != end(range);
}

TrackMediaFrameIterator::TrackMediaFrameIterator(const Timeline * pTimeline, size_t currentFrame) {
	reset(pTimeline, currentFrame);
}
void TrackMediaFrameIterator::reset(const Timeline * pTimeline, size_t currentFrame) {
	m_References.clear();
	if (pTimeline)
		for (const Track& track : *pTimeline) {
			const MediaFrameReference mfr = track.getMediaFrameReferenceAt(currentFrame);
			if (mfr != EMPTY)
				m_References.push_back(mfr);
		}
}
void TrackMediaFrameIterator::clear() {
	m_References.clear();
}
MediaFrameReference TrackMediaFrameIterator::next() {
	assert(!empty());
	MediaFrameReference mfr = m_References.front();
	m_References.erase(m_References.begin());
	return mfr;
}
bool TrackMediaFrameIterator::empty() const {
	return m_References.empty();
}

FrameIterator::FrameIterator(const Ranges *pMediaRanges, size_t initialFrame, IterationMode mode) :
		m_pMediaRanges(pMediaRanges), m_Mode(mode), m_FramesToGo(-1), m_bForward(m_Mode == IterationMode::FORWARD) {
	if (!empty()) {
		m_FramesToGo = 0;
		const Range span(m_pMediaRanges->front().first, m_pMediaRanges->back().last);
		m_SpanCount = span.count();
		size_t lowerFrame = span.first;
		size_t upperFrame = span.last;
		for (const Range& range : *m_pMediaRanges) {
			m_FramesToGo += range.count();
			if (initialFrame >= range.first && initialFrame <= range.last)
				lowerFrame = upperFrame = initialFrame;
			else if (initialFrame > range.last && lowerFrame < range.last)
				lowerFrame = range.last;
			else if (initialFrame < range.first && upperFrame > range.first)
				upperFrame = range.first;
		}
		if (lowerFrame == upperFrame) {
			if (m_Mode == IterationMode::PINGPONG)
				m_bForward = false;
			if (m_bForward) {
				m_Forward = upperFrame;
				m_Backward = findPrevious(m_Forward);
			} else {
				m_Backward = upperFrame;
				m_Forward = findNext(m_Backward);
			}
		} else {
			if (m_Mode == IterationMode::PINGPONG)
				m_bForward = true;
			m_Forward = upperFrame;
			m_Backward = lowerFrame;
		}
	}
}
void FrameIterator::clear() {
	m_pMediaRanges = nullptr;
}
size_t FrameIterator::next() {
	assert(!empty());
	size_t current = m_bForward ? m_Forward : m_Backward;
	if (m_bForward)
		m_Forward = findNext(m_Forward);
	else
		m_Backward = findPrevious(m_Backward);
	if (m_Mode == IterationMode::PINGPONG)
		m_bForward = !m_bForward;
	--m_FramesToGo;
	return current;
}
bool FrameIterator::empty() const {
	return m_pMediaRanges == nullptr || m_pMediaRanges->empty() || m_FramesToGo == 0;
}
size_t FrameIterator::findNext(size_t frame) const {
	size_t nextAbsoluteFrame = frame + 1 == m_SpanCount ? 0 : frame + 1;
	for (const Range& range : *m_pMediaRanges) {
		if (range.last < nextAbsoluteFrame)
			continue;
		if (range.first < nextAbsoluteFrame)
			break;
		nextAbsoluteFrame = range.first;
		break;
	}
	return nextAbsoluteFrame;
}
size_t FrameIterator::findPrevious(size_t frame) const {
	size_t nextAbsoluteFrame = frame == 0 ? m_SpanCount - 1 : frame - 1;
	for (auto itr = m_pMediaRanges->rbegin(); itr != m_pMediaRanges->rend(); ++itr) {
		const Range& range = *itr;
		if (range.first > nextAbsoluteFrame)
			continue;
		if (range.last > nextAbsoluteFrame)
			break;
		nextAbsoluteFrame = range.last;
		break;
	}
	return nextAbsoluteFrame;
}

static const Timeline* nullIfEmpty(const Timeline * pTimeline) {
	if (pTimeline == nullptr || pTimeline->empty())
		return nullptr;
	return pTimeline;
}

TimelineIterator::TimelineIterator() :
		TimelineIterator(nullptr, nullptr, 0) {
}

TimelineIterator::TimelineIterator(const Timeline * pTimeline, const Ranges *pMediaRanges, size_t currentFrame) :
		m_pTimeline(nullIfEmpty(pTimeline)), //
		m_pMediaRanges(pMediaRanges), //
		m_CurrentFrame(currentFrame), //
		m_CurrentTrackIndex(0) {
	if (!empty() && !valid())
		stepUntilValidOrExhausted();
	m_EndFrame = m_CurrentFrame;
}

bool TimelineIterator::empty() {
	return m_pMediaRanges == nullptr || m_pMediaRanges->empty();
}

void TimelineIterator::clear() {
	m_pTimeline = nullptr;
	m_pMediaRanges = nullptr;
}

MediaFrameReference TimelineIterator::next() {
	assert(m_pTimeline);
	assert(m_pMediaRanges);
	MediaFrameReference reference = getCurrentTrack().getMediaFrameReferenceAt(m_CurrentFrame);
	stepUntilValidOrExhausted();
	return reference;
}

size_t TimelineIterator::getCurrentFrame() const {
	return m_CurrentFrame;
}

void TimelineIterator::stepUntilValidOrExhausted() {
	assert(!empty());
	do {
		stepForward();
		if (finished()) {
			clear();
			return;
		}
	} while (!valid());
}

bool TimelineIterator::finished() const {
	return m_CurrentFrame == m_EndFrame && m_CurrentTrackIndex == 0;
}

void TimelineIterator::regularizeCurrentFrame() {
	if (contains(*m_pMediaRanges, m_CurrentFrame))
		return;
	if (m_CurrentFrame > m_pMediaRanges->back().last) {
		m_CurrentFrame = m_pMediaRanges->front().first;
	} else {
		const auto pFound = std::find_if(m_pMediaRanges->rbegin(), m_pMediaRanges->rend(), [=](const Range &range) {
			return m_CurrentFrame< range.first;
		});
		m_CurrentFrame = pFound->first;
	}
}

void TimelineIterator::stepForward() {
	++m_CurrentTrackIndex;
	if (m_CurrentTrackIndex == m_pTimeline->size()) {
		m_CurrentTrackIndex = 0;
		++m_CurrentFrame;
		regularizeCurrentFrame();
	}
}

bool TimelineIterator::valid() const {
	const Track &track = getCurrentTrack();
	const auto pFound = track.clipContaining(m_CurrentFrame);
	return pFound == track.end() ? false : pFound->second.pStream != nullptr;
}

const Track& TimelineIterator::getCurrentTrack() const {
	return (*m_pTimeline)[m_CurrentTrackIndex];
}

}
/* namespace duke */
