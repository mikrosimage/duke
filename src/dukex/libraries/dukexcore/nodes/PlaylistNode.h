#ifndef PLAYLISTNODE_H
#define PLAYLISTNODE_H

#include <dukexcore/dkxINode.h>
#include <dukexcore/dkxSessionDescriptor.h>
#include <dukeapi/core/PlaylistReader.h>
#include "player.pb.h"


class PlaylistNode : public INode {

public:
    typedef boost::shared_ptr<PlaylistNode> ptr;
    PlaylistNode() :
        INode("fr.mikrosimage.dukex.playlist") {
    }

public:
    bool open(const std::string & _filename) {
        try {
            SessionDescriptor & descriptor = session()->descriptor();
            ::duke::protocol::Playlist & p = descriptor.playlist();
            p.Clear();
            p.set_frameratenumerator(25);
            p.set_playbackmode(::duke::protocol::Playlist::DROP_FRAME_TO_KEEP_REALTIME);
            MessageQueue queue;
            int clipIndex = 0;
            int recIn = 0;
            PlaylistReader reader(clipIndex, recIn, _filename, queue, p);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool setFramerate(float framerate) {
        try {
            SessionDescriptor & descriptor = session()->descriptor();
            ::duke::protocol::Playlist & p = descriptor.playlist();
            p.set_frameratenumerator((int)framerate);
            MessageQueue queue;
            push(queue, p);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool debug() {
        try {
            MessageQueue queue;
            ::duke::protocol::Debug d;
            d.add_line("%0");
            d.add_line("%1");
            d.add_line("%2");
            d.add_content(::duke::protocol::Debug_Content_FRAME);
            d.add_content(::duke::protocol::Debug_Content_FILENAMES);
            d.add_content(::duke::protocol::Debug_Content_FPS);
            push(queue, d, google::protobuf::serialize::MessageHolder::RETRIEVE);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
};

#endif // PLAYLISTNODE_H
