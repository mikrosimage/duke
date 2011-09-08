/*
 * PlaylistHelper.h
 *
 *  Created on: 14 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef PLAYLISTHELPER_H_
#define PLAYLISTHELPER_H_

#include "ClipHelper.h"

#include <communication.pb.h>

#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>

struct PlaylistHelper : boost::noncopyable {
private:
    ::protocol::duke::Playlist m_Playlist;
    size_t m_uRecIn;
    size_t m_uRecOut;
    size_t m_uFrameCount;
    size_t m_uEndIterator;

    // accelerators typedef
    typedef boost::ptr_vector<ClipHelper> ClipHelpers;
    typedef std::vector<size_t> Indices;
    typedef std::pair<size_t, Indices> MapValue;
    // members
    ClipHelpers m_vpClipHelpers;
public:
    typedef std::map<size_t, MapValue> FrameToIndices;
    typedef std::map<size_t, FrameToIndices::const_iterator> IteratorToFrame;
    // members
    FrameToIndices m_mFrameToClipIndices;
    IteratorToFrame m_mIteratorToFrame;
public:
    // functions
    PlaylistHelper();
    PlaylistHelper(const ::protocol::duke::Playlist& playlist);
    void getClipsAtFrame(const size_t frame, std::vector<protocol::duke::Clip>& clips) const;
    void getIteratorsAtFrame(const size_t frame, std::vector<size_t>& indices) const;
    size_t getIteratorIndexAtFrame(const size_t frame) const;
    boost::filesystem::path getPathAtIterator(const size_t) const;
    boost::filesystem::path getPathAtHash(const uint64_t) const;
    uint64_t getHashAtIterator(const size_t) const;
    size_t getWrappedFrame(size_t frame) const;
    size_t getClampedFrame(size_t frame) const;
    size_t getNormalizedFrame(size_t frame) const;
    inline const ::protocol::duke::Playlist& getPlaylist() const {
        return m_Playlist;
    }
    inline size_t getRecIn() const {
        return m_uRecIn;
    }
    inline size_t getRecOut() const {
        return m_uRecOut;
    }
    inline size_t getFirstFrame() const {
        return m_uRecIn;
    }
    inline size_t getLastFrame() const {
        return m_uRecOut - 1;
    }
    inline size_t getFrameCount() const {
        return m_uFrameCount;
    }
    inline size_t getEndIterator() const {
        return m_uEndIterator;
    }
    inline const Indices& getIndices(const FrameToIndices::const_iterator &itr) const {
        return itr->second.second;
    }
    inline const size_t& getItr(const FrameToIndices::const_iterator &itr) const {
        return itr->second.first;
    }
    inline const size_t& getFrame(const FrameToIndices::const_iterator &itr) const {
        return itr->first;
    }
    const ClipHelper* getClipHelperFrom(const size_t index, size_t &frameInClip) const;

    inline void swap(PlaylistHelper&& other) {
        std::swap(m_Playlist, other.m_Playlist);
        std::swap(m_uRecIn, other.m_uRecIn);
        std::swap(m_uRecOut, other.m_uRecOut);
        std::swap(m_uFrameCount, other.m_uFrameCount);
        std::swap(m_uEndIterator, other.m_uEndIterator);
        std::swap(m_vpClipHelpers, other.m_vpClipHelpers);
        std::swap(m_mFrameToClipIndices, other.m_mFrameToClipIndices);
        std::swap(m_mIteratorToFrame, other.m_mIteratorToFrame);
    }
private:
    inline Indices& getIndices(FrameToIndices::iterator &itr) {
        return itr->second.second;
    }
    inline size_t& getItr(const FrameToIndices::iterator &itr) {
        return itr->second.first;
    }
    inline FrameToIndices::iterator insertEmptyValue(const size_t key, const Indices &indices) {
        return m_mFrameToClipIndices.insert(std::make_pair(key, std::make_pair(0, indices))).first;
    }
    void dump(const FrameToIndices&) const;
    void dump(const IteratorToFrame&) const;
    FrameToIndices::iterator ensureKey(const size_t key);
    FrameToIndices::const_iterator getRangeContainingFrame(const size_t frame) const;
    FrameToIndices::const_iterator getRangeContainingItr(const size_t iteratorIndex) const;
};

typedef boost::shared_ptr<PlaylistHelper> SharedPlaylistHelperPtr;

#endif /* PLAYLISTHELPER_H_ */
