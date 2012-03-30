#ifndef DKXSESSIONDESCRIPTOR_H
#define DKXSESSIONDESCRIPTOR_H

#include "player.pb.h"
#include "playlist.pb.h"
#include <dukeapi/QueueMessageIO.h>
#include <boost/noncopyable.hpp>

class SessionDescriptor : private boost::noncopyable {

public:
    SessionDescriptor() :
        mFrame(0), mFramerate(25.), mPlaying(false), mCacheSize(0), mThreadSize(1) {
    }

public:
    inline const size_t currentFrame() const {
        return mFrame;
    }
    inline void setCurrentFrame(const size_t f) {
        mFrame = f;
    }
    inline const double framerate() const {
        return mFramerate;
    }
    inline void setFramerate(double f) {
        mFramerate = f;
    }
    inline const bool isPlaying() const {
        return mPlaying;
    }
    inline void setIsPlaying(const bool b) {
        mPlaying = b;
    }
    inline const uint64_t cacheSize() const {
        return mCacheSize;
    }
    inline void setCacheSize(const uint64_t size) {
        mCacheSize = size;
    }
    inline const size_t threadSize() const {
        return mThreadSize;
    }
    inline void setThreadSize(const size_t size) {
        mThreadSize = size;
    }
    inline const std::string rendererPath() const {
        return mRendererPath;
    }
    inline void setRendererPath(const std::string& path) {
        mRendererPath = path;
    }
    ::duke::playlist::Playlist & playlist() {
        return mPlaylist;
    }
    const ::duke::playlist::Playlist & playlist() const {
        return mPlaylist;
    }
    void setPlaylist(const ::duke::playlist::Playlist & p) {
        mPlaylist = p;
    }
    inline ::duke::protocol::StaticParameter & displayMode() {
        return mDisplayMode;
    }
    inline const ::duke::protocol::StaticParameter & displayMode() const {
        return mDisplayMode;
    }
    inline const ::duke::protocol::PlaybackState::PlaybackMode & playbackMode() const {
        return mPlaybackMode;
    }
    inline MessageQueue & getInitTimeQueue() {
        return mInitTimeQueue;
    }
private:
    bool mConnected;
    size_t mFrame;
    double mFramerate;
    bool mPlaying;
    uint64_t mCacheSize;
    size_t mThreadSize;
    std::string mRendererPath;
    ::duke::playlist::Playlist mPlaylist;
    ::duke::protocol::StaticParameter mDisplayMode;
    ::duke::protocol::PlaybackState::PlaybackMode mPlaybackMode;
    MessageQueue mInitTimeQueue;
};

#endif // DKXSESSIONDESCRIPTOR_H
