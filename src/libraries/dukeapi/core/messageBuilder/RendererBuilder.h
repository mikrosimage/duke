/*
 * RendererBuilder.h
 *
 *  Created on: 14 avr. 2011
 *      Author: Nicolas Rondaud
 */

#ifndef RENDERERBUILDER_H
#define RENDERERBUILDER_H

#include <duke_pb/IMessageIO.h>
#include <communication.pb.h>
#include <iostream>

namespace {

namespace dk = ::protocol::duke;

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
    dk::Quit q;
    q.set_returncode(0);
    push(_queue, q);
}

void quitRenderer(IMessageIO & _queue) {
    dk::Quit q;
    q.set_returncode(-1);
    push(_queue, q);
}

} // empty namespace

#endif /* RENDERERBUILDER_H */
