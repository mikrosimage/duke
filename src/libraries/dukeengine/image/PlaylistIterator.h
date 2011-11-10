/*
 * PlaylistIterator.h
 *
 *  Created on: 3 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef PLAYLISTITERATOR_H_
#define PLAYLISTITERATOR_H_

#include <dukeengine/range/Range.h>
#include <dukeengine/range/PlaylistRange.h>
#include <dukeengine/sequence/PlaylistHelper.h>

struct PlaylistIterator : public OnePassRange<uint64_t> {
    PlaylistIterator(const PlaylistHelper& helper, uint32_t fromFrame, int32_t speed) :
        m_Helper(helper), m_FrameRange(build(helper, fromFrame, speed)) {
        advanceFrame();
    }

    virtual bool empty() const {
        return m_CurrentFrame == SENTINEL && m_CurrentFrameIteratorIndex == SENTINEL;
    }

    virtual void popFront() {
        assert(!empty());
        ++m_CurrentFrameIteratorIndex;
        if (isLastIterator())
            advanceFrame();
    }

    virtual uint64_t front() {
        assert(m_CurrentFrame!=size_t(-1));
        assert(m_CurrentFrameIteratorIndex!=size_t(-1));
        assert(m_CurrentFrameIteratorIndex<m_IteratorsAtCurrentFrame.size());
        return m_Helper.getHashAtIterator(m_IteratorsAtCurrentFrame[m_CurrentFrameIteratorIndex]);
    }

private:
    inline static range::LimitedPlaylistFrameRange build(const PlaylistHelper& helper, uint32_t fromFrame, int32_t speed) {
        if (helper.getEndIterator() == 0)
            return range::LimitedPlaylistFrameRange(0, 0, fromFrame, speed);
        return range::LimitedPlaylistFrameRange(helper.getFirstFrame(), helper.getLastFrame(), fromFrame, speed);
    }

    inline void advanceFrame() {
        m_CurrentFrame = SENTINEL;
        m_CurrentFrameIteratorIndex = SENTINEL;
        if (m_FrameRange.empty())
            return;
        m_CurrentFrame = m_FrameRange.front();
        m_FrameRange.popFront();
        m_Helper.getIteratorsAtFrame(m_CurrentFrame, m_IteratorsAtCurrentFrame);
        m_CurrentFrameIteratorIndex = 0;
    }

    inline bool isLastIterator() {
        return m_CurrentFrameIteratorIndex >= m_IteratorsAtCurrentFrame.size();
    }

    static const size_t SENTINEL = -1;
    PlaylistHelper m_Helper;
    range::LimitedPlaylistFrameRange m_FrameRange;

    std::size_t m_CurrentFrame;
    std::vector<size_t> m_IteratorsAtCurrentFrame;
    std::size_t m_CurrentFrameIteratorIndex;
};

struct IndexedPlaylistIterator : public PlaylistIterator {
    IndexedPlaylistIterator(const PlaylistHelper& helper, uint32_t fromFrame, int32_t speed) :
        PlaylistIterator(helper, fromFrame, speed), m_CurrentIndex(0), m_Limited(false) {
    }

    virtual bool empty() const {
        return m_Limited || PlaylistIterator::empty();
    }

    virtual void popFront() {
        PlaylistIterator::popFront();
        ++m_CurrentIndex;
    }

    size_t index() const {
        return m_CurrentIndex;
    }

    void limitHere() {
        m_Limited = true;
    }
private:
    size_t m_CurrentIndex;
    bool m_Limited;
};
#endif /* PLAYLISTITERATOR_H_ */
