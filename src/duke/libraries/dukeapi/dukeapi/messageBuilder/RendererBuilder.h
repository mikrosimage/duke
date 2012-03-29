#ifndef RENDERERBUILDER_H
#define RENDERERBUILDER_H

#include "QuitBuilder.h"
#include <dukeapi/IMessageIO.h>
#include <player.pb.h>
#include <iostream>

namespace {

void quitRenderer(IMessageIO & _queue) {
    using namespace ::google::protobuf::serialize;
    _queue.push(make_shared(quitSuccess()));
}

} // empty namespace

#endif /* RENDERERBUILDER_H */
