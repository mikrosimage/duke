#ifndef DKXSESSIONDESCRIPTOR_H
#define DKXSESSIONDESCRIPTOR_H

#include "player.pb.h"

class SessionDescriptor {

public:
    SessionDescriptor() {
    }
    SessionDescriptor(const SessionDescriptor& d) :
        mRenderer(d.renderer()), mPlaylist(d.playlist()), mDisplayMode(d.displayMode()) {
    }
    const SessionDescriptor& operator=(const SessionDescriptor& d) {
        mRenderer = d.renderer();
        mPlaylist = d.playlist();
        mDisplayMode = d.displayMode();
        return *this;
    }

public:
    inline ::duke::protocol::Renderer & renderer() {
        return mRenderer;
    }
    inline const ::duke::protocol::Renderer & renderer() const {
        return mRenderer;
    }
    inline ::duke::protocol::Playlist & playlist() {
        return mPlaylist;
    }
    inline const ::duke::protocol::Playlist & playlist() const {
        return mPlaylist;
    }
    inline ::duke::protocol::StaticParameter & displayMode() {
        return mDisplayMode;
    }
    inline const ::duke::protocol::StaticParameter & displayMode() const {
        return mDisplayMode;
    }

private:
    ::duke::protocol::Renderer mRenderer;
    ::duke::protocol::Playlist mPlaylist;
    ::duke::protocol::StaticParameter mDisplayMode;
};

#endif // DKXSESSIONDESCRIPTOR_H
