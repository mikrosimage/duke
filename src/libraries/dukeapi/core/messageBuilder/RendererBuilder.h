#ifndef RENDERERBUILDER_H
#define RENDERERBUILDER_H

#include "QuitBuilder.h"
#include <dukeapi/io/IMessageIO.h>
#include <player.pb.h>
#include <iostream>

namespace {

namespace dk = ::duke::protocol;

void buildRenderer(dk::Renderer& _r //
                   , bool _windowed = true//
                   , size_t _width = 800 //
                   , size_t _height = 600 //
                   , void * _handle = NULL) {
    _r.set_fullscreen(!_windowed);
    _r.set_width(_width);
    _r.set_height(_height);
    if (_handle)
        _r.set_handle((::google::protobuf::uint64) _handle);

}

void startRenderer(IMessageIO & _queue //
                   , bool _windowed = true //
                   , size_t _width = 800 //
                   , size_t _height = 600 //
                   , void * _handle = NULL) {
    dk::Renderer r;
    buildRenderer(r, _windowed, _width, _height, _handle);
    push(_queue, r);
}

void stopRenderer(IMessageIO & _queue) {
    using namespace ::google::protobuf::serialize;
    _queue.push(makeSharedHolder(relaunch()));
}

void quitRenderer(IMessageIO & _queue) {
    using namespace ::google::protobuf::serialize;
    _queue.push(makeSharedHolder(quitSuccess()));
}

} // empty namespace

#endif /* RENDERERBUILDER_H */
