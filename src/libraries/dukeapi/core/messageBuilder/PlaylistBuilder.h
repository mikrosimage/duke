#ifndef PLAYLISTBUILDER_H_
#define PLAYLISTBUILDER_H_

#include <dukeapi/core/IMessageIO.h>
#include <dukeapi/protocol/player/protocol.pb.h>
#include <iostream>

namespace {

namespace dk = ::duke::protocol;

void buildPlaylist(dk::Playlist& _p //
                   , size_t _framerate = 25//
                   , bool _loop = true //
                   , dk::Playlist_PlaybackMode _playbackmode = dk::Playlist::DROP_FRAME_TO_KEEP_REALTIME) {
    _p.set_frameratenumerator(_framerate);
    _p.set_loop(_loop);
    _p.set_playbackmode(_playbackmode);
}

void addSimplePlaylist(IMessageIO & _queue //
                       , size_t _framerate = 25 //
                       , bool _loop = true //
                       , dk::Playlist_PlaybackMode _playbackmode = dk::Playlist::DROP_FRAME_TO_KEEP_REALTIME) {
    dk::Playlist p;
    buildPlaylist(p, _framerate, _loop, _playbackmode);
    push(_queue, p);
}

} // empty namespace

#endif /* PLAYLISTBUILDER_H_ */
