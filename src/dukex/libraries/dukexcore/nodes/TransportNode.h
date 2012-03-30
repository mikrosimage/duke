#ifndef TRANSPORTNODE_H
#define TRANSPORTNODE_H

#include <dukexcore/dkxINode.h>
#include "player.pb.h"

namespace {

::duke::protocol::Transport MAKE(const ::duke::protocol::Transport_TransportType type //
                                 , const int value = -1 //
                                 , const bool cueRelative = false //
                                 , const bool cueClip = false) {
    ::duke::protocol::Transport transport;
    transport.set_type(type);
    transport.set_autonotifyonframechange(true);
    if (type == ::duke::protocol::Transport_TransportType_CUE) {
        if (!cueRelative && value < 0)
            throw std::runtime_error("can't cue to a negative frame");
        ::duke::protocol::Transport_Cue *cue = transport.mutable_cue();
        cue->set_cueclip(cueClip);
        cue->set_cuerelative(cueRelative);
        cue->set_value(value);
    }
    return transport;
}

} // namespace


class TransportNode : public INode {

public:
    typedef boost::shared_ptr<TransportNode> ptr;
    TransportNode() :
        INode("fr.mikrosimage.dukex.transport") {
    }

public:
    void play() {
        MessageQueue q;
        push(q, MAKE(::duke::protocol::Transport_TransportType_PLAY));
        session()->sendMsg(q);
        session()->descriptor().setIsPlaying(true);
    }

    void stop() {
        MessageQueue q;
        push(q, MAKE(::duke::protocol::Transport_TransportType_STOP));
        session()->sendMsg(q);
        session()->descriptor().setIsPlaying(false);
    }

    void gotoFrame(size_t _f) {
        MessageQueue q;
        push(q, MAKE(::duke::protocol::Transport_TransportType_CUE, _f));
        session()->sendMsg(q);
    }

    void previousFrame() {
        MessageQueue q;
        push(q, MAKE(::duke::protocol::Transport_TransportType_CUE, -1, true));
        session()->sendMsg(q);
    }

    void nextFrame() {
        MessageQueue q;
        push(q, MAKE(::duke::protocol::Transport_TransportType_CUE, 1, true));
        session()->sendMsg(q);
    }

    void firstFrame() {
        MessageQueue q;
        push(q, MAKE(::duke::protocol::Transport_TransportType_CUE_FIRST));
        session()->sendMsg(q);
    }

    void lastFrame() {
        MessageQueue q;
        push(q, MAKE(::duke::protocol::Transport_TransportType_CUE_LAST));
        session()->sendMsg(q);
    }

    void previousShot() {
        MessageQueue q;
        push(q, MAKE(::duke::protocol::Transport_TransportType_CUE, -1, true, true));
        session()->sendMsg(q);
    }

    void nextShot() {
        MessageQueue q;
        push(q, MAKE(::duke::protocol::Transport_TransportType_CUE, 1, true, true));
        session()->sendMsg(q);
    }
};

#endif // TRANSPORTNODE_H
