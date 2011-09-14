#include "PlaylistHelper.h"
#include <dukeengine/utils/TimeUtils.h>
#include <boost/foreach.hpp>
#include <boost/integer_traits.hpp>

#include <iterator>

#include <iostream>
using namespace std;

PlaylistHelper::PlaylistHelper() :
    m_Playlist(), m_uRecIn(0), m_uRecOut(0), m_uFrameCount(0), m_uEndIterator(0) {
}

PlaylistHelper::PlaylistHelper(const ::duke::protocol::Playlist& _playlist) :
    m_Playlist(_playlist), m_uRecIn(boost::integer_traits<uint32_t>::const_max), m_uRecOut(boost::integer_traits<uint32_t>::const_min), m_uEndIterator(0) {
    using namespace std;

    const int clipCount = m_Playlist.clip_size();

    if (clipCount == 0) {
        m_uRecIn = 0;
        m_uRecOut = 0;
    }

    m_vpClipHelpers.reserve(clipCount);
    using namespace ::duke::protocol;
    using namespace ::google::protobuf;

    for (RepeatedPtrField<Clip>::const_iterator itr = m_Playlist.clip().begin(), end = m_Playlist.clip().end(); itr != end; ++itr) {
        const Clip &clip = *itr;
        const size_t recin = clip.recin();
        const size_t recout = clip.recout();
        // adjusting playlist bounds
        if (recin < m_uRecIn)
            m_uRecIn = recin;
        if (recout > m_uRecOut)
            m_uRecOut = recout;
        // keeping a clip helper
        m_vpClipHelpers.push_back(new ClipHelper(clip));
        const size_t currentClipHelperIndex = m_vpClipHelpers.size() - 1;
        // filling the accelerator map
        FrameToIndices::iterator frameInItr = ensureKey(recin);
        FrameToIndices::iterator frameOutItr = ensureKey(recout);
        // adding this clipHelperIndex to all the values between first and last iterator
        for (; frameInItr != frameOutItr; ++frameInItr)
            getIndices(frameInItr).push_back(currentClipHelperIndex);
    }
    // now building the iterator accelerator
    const FrameToIndices::iterator beginFrame = m_mFrameToClipIndices.begin();
    const FrameToIndices::iterator endFrame = m_mFrameToClipIndices.end();
    FrameToIndices::iterator currentFrame = beginFrame;
    FrameToIndices::iterator previousFrame;
    IteratorToFrame::iterator previousItr = m_mIteratorToFrame.begin();
    for (; currentFrame != endFrame; ++currentFrame) {
        size_t iterator = 0;
        if (!m_mIteratorToFrame.empty()) {
            const size_t lastFrameAdjacentDifference = currentFrame->first - previousFrame->first;
            const size_t howManyClipOnPreviousSlot = previousFrame->second.second.size();
            const size_t frameMultiplier = max(size_t(1), howManyClipOnPreviousSlot);
            iterator = previousItr->first + (lastFrameAdjacentDifference * frameMultiplier);
        }
        getItr(currentFrame) = iterator;
        previousItr = m_mIteratorToFrame.insert(make_pair(iterator, currentFrame)).first;
        previousFrame = currentFrame;
    }
    //dump(m_mFrameToClipIndices);
    m_uFrameCount = m_uRecOut - m_uRecIn;
    if (!m_mIteratorToFrame.empty())
        m_uEndIterator = m_mIteratorToFrame.rbegin()->first;
}

void PlaylistHelper::getClipsAtFrame(const size_t frame, std::vector<duke::protocol::Clip>& clips) const {
    clips.clear();
    const FrameToIndices::const_iterator itr = getRangeContainingFrame(frame);
    if (itr == m_mFrameToClipIndices.end())
        return;
    for (Indices::const_iterator it = getIndices(itr).begin(), end = getIndices(itr).end(); it != end; ++it)
        clips.push_back(m_Playlist.clip(*it));
}

void PlaylistHelper::getIteratorsAtFrame(const size_t frame, std::vector<size_t>& indices) const {
    indices.clear();
    const FrameToIndices::const_iterator frameItr = getRangeContainingFrame(frame);
    if (frameItr == m_mFrameToClipIndices.end())
        return;
    const size_t baseFrame = getFrame(frameItr);
    const size_t frameDiff = frame - baseFrame;
    const size_t clipCount = getIndices(frameItr).size();
    const size_t baseItr = getItr(frameItr) + frameDiff * clipCount;
    for (size_t i = 0; i < clipCount; ++i)
        indices.push_back(baseItr + i);
}

PlaylistHelper::FrameToIndices::const_iterator PlaylistHelper::getRangeContainingFrame(const size_t frame) const {
    const FrameToIndices::const_iterator end = m_mFrameToClipIndices.end();
    if (frame < m_uRecIn || frame > m_uRecOut)
        return end;
    FrameToIndices::const_iterator lower = m_mFrameToClipIndices.lower_bound(frame);
    if (lower->first == frame) // good key returning it
        return lower;
    // we are on the next value
    // lower!=end() because recout is in the map and frame<=recout
    // so last statement would have returned
    assert(lower != end);
    // we can safely return the previous value because
    // frame >= recin && recin is in the map
    // so last statement would have returned
    return --lower;
}

PlaylistHelper::FrameToIndices::const_iterator PlaylistHelper::getRangeContainingItr(const size_t iteratorIndex) const {
    const IteratorToFrame::const_iterator end = m_mIteratorToFrame.end();
    if (iteratorIndex > m_uEndIterator)
        return m_mFrameToClipIndices.end();
    IteratorToFrame::const_iterator lower = m_mIteratorToFrame.lower_bound(iteratorIndex);
    if (lower->first == iteratorIndex) // good key returning it
        return lower->second;
    // we are on the next value
    // lower!=end() because m_uEndIterator is in the map and iteratorIndex<=m_uEndIterator
    // so last statement would have returned
    assert(lower != end);
    // we can safely return the previous value because
    // iteratorIndex >= 0 && 0 is in the map
    // so last statement would have returned
    return (--lower)->second;
}

size_t PlaylistHelper::getIteratorIndexAtFrame(const size_t frame) const {
    const FrameToIndices::const_iterator frameItr = getRangeContainingFrame(frame);
    if (frameItr == m_mFrameToClipIndices.end() || getIndices(frameItr).empty())
        return m_uEndIterator;
    const size_t baseFrame = getFrame(frameItr);
    const size_t frameDiff = frame - baseFrame;
    return getItr(frameItr) + frameDiff * getIndices(frameItr).size();
}

const ClipHelper* PlaylistHelper::getClipHelperFrom(const size_t index, size_t &frameInClip) const {
    const FrameToIndices::const_iterator frameItr = getRangeContainingItr(index);
    if (frameItr == m_mFrameToClipIndices.end())
        return NULL;
    const size_t baseFrame = getFrame(frameItr);
    const size_t baseIndex = getItr(frameItr);
    assert(baseIndex <= index);
    const size_t indexDiff = index - baseIndex;
    const Indices &indices(getIndices(frameItr));
    const size_t clipCount = indices.size();
    if (clipCount == 0)
        return NULL;
    const size_t frameDiff = indexDiff / clipCount;
    const size_t realFrameToCheck = baseFrame + frameDiff;
    const size_t clipIndexInVector = indexDiff % clipCount;
    const size_t clipIndex = indices[clipIndexInVector];
    frameInClip = realFrameToCheck;
    return &m_vpClipHelpers[clipIndex];
}

boost::filesystem::path PlaylistHelper::getPathAtIterator(const size_t index) const {
    using namespace boost::filesystem;
    size_t frameInClip;
    const ClipHelper *pHelper = getClipHelperFrom(index, frameInClip);
    if (pHelper == NULL)
        return path();
    return pHelper->getFilenameAt(frameInClip);
}

uint64_t PlaylistHelper::getHashAtIterator(const size_t index) const {
    size_t frameInClip;
    const ClipHelper *pHelper = getClipHelperFrom(index, frameInClip);
    if (pHelper == NULL)
        return 0;
    return pHelper->getHashAt(frameInClip);
}

boost::filesystem::path PlaylistHelper::getPathAtHash(const uint64_t hash) const {
    assert(hash != 0);
    const size_t clipHash = hash >> 32;
    const size_t clipIndex = hash & 0xFFFFFFFF;
    for (ClipHelpers::const_iterator itr = m_vpClipHelpers.begin(); itr != m_vpClipHelpers.end(); ++itr)
        if (itr->getClipHash() == clipHash)
            return itr->getFilenameAtFrame(clipIndex);
    return boost::filesystem::path();
}

PlaylistHelper::FrameToIndices::iterator PlaylistHelper::ensureKey(const size_t key) {
    assert(key >= 0);
    // container is empty ?
    if (m_mFrameToClipIndices.empty()) //insert empty vector
        return insertEmptyValue(key, Indices());
    // container is not empty
    FrameToIndices::iterator itr = m_mFrameToClipIndices.lower_bound(key);
    // we are not past the end of the container
    if (itr != m_mFrameToClipIndices.end()) {
        if (itr->first == key) // good key, returning it
            return itr;
        // we are in the middle of the container and container is not empty
        Indices &lastIndices(getIndices(--itr)); // taking the value of previous element
        return insertEmptyValue(key, lastIndices);
    }
    // we are past the end of the container and container is not empty
    Indices &lastIndices(m_mFrameToClipIndices.rbegin()->second.second); // taking the value of the last element
    return insertEmptyValue(key, lastIndices);
}

void PlaylistHelper::dump(const FrameToIndices&map) const {
    using namespace std;

    cerr << "dumping map" << endl;
    BOOST_FOREACH( const FrameToIndices::value_type& pair, map )
                {
                    cerr << pair.first << " : ";
                    cerr << pair.second.first << " : ";
                    copy(pair.second.second.begin(), pair.second.second.end(), ostream_iterator<size_t> (cerr, " "));
                    cerr << endl;
                }
}
void PlaylistHelper::dump(const IteratorToFrame&map) const {
    using namespace std;

    cerr << "dumping map" << endl;
    BOOST_FOREACH( const IteratorToFrame::value_type& pair, map )
                {
                    cerr << pair.first << " : " << pair.second->first << endl;
                }
}

size_t PlaylistHelper::getWrappedFrame(size_t frame) const {
    // due to size_t being an unsigned type we need
    // to guaranty frame is >= to recIn to perform
    // the calculation right.
    const size_t frameCount = getFrameCount();
    if (frame == std::numeric_limits<size_t>::max())
        return frameCount - 1;

    if (frameCount == 0)
        return 0;
    const size_t recIn = getRecIn();
    while (frame < recIn)
        frame += frameCount;

    const size_t howManyPeriod = (size_t) ((frame - recIn) / frameCount);
    frame -= howManyPeriod * frameCount;

    return frame;
}

size_t PlaylistHelper::getClampedFrame(size_t frame) const {
    return std::max(std::min(frame, getRecOut() - 1), getRecIn());
}

size_t PlaylistHelper::getNormalizedFrame(size_t frame) const {
    return getPlaylist().loop() ? getWrappedFrame(frame) : getClampedFrame(frame);
}
