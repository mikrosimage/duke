#ifndef PLAYBACKNODE_H
#define PLAYBACKNODE_H

#include <dukexcore/dkxINode.h>
#include "player.pb.h"

class PlaybackNode : public INode {

public:
    typedef boost::shared_ptr<PlaybackNode> ptr;
    PlaybackNode() :
        INode("fr.mikrosimage.dukex.playback") {
    }

public:
    void registerFramerate(float framerate) {
        SessionDescriptor & descriptor = session()->descriptor();
        descriptor.setFramerate(framerate);
    }

    bool setFramerate(float framerate) {
        try {
            // register new framerate value
            registerFramerate(framerate);

            MessageQueue queue;
            duke::protocol::PlaybackState playback;
            playback.set_frameratenumerator(framerate);
            push(queue, playback);

            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }


};

#endif // PLAYBACKNODE_H
