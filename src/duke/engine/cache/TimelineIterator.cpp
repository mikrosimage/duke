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

struct RangeComparator : std::binary_function<Range, Range, bool> {
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
    return std::any_of(range.begin(), range.end(), [=](const Range &range) {
        return range.first<=frame && range.last>=frame;
    });
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
