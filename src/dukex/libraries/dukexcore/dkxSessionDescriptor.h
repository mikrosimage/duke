#ifndef DKXSESSIONDESCRIPTOR_H
#define DKXSESSIONDESCRIPTOR_H

#include "player.pb.h"

class SessionDescriptor {

public:
    SessionDescriptor(){}

public:
    inline ::duke::protocol::Renderer & renderer() {
        return mRenderer;
    }
    inline ::duke::protocol::Playlist & playlist() {
        return mPlaylist;
    }
    inline ::duke::protocol::StaticParameter & displayMode() {
        return mDisplayMode;
    }

private:
    SessionDescriptor(const SessionDescriptor&);
    const SessionDescriptor& operator=(const SessionDescriptor&);

private:
    ::duke::protocol::Renderer mRenderer;
    ::duke::protocol::Playlist mPlaylist;
    ::duke::protocol::StaticParameter mDisplayMode;
};

#endif // DKXSESSIONDESCRIPTOR_H
