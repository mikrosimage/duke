#ifndef PLAYLISTNODE_H
#define PLAYLISTNODE_H

#include <dukexcore/dkxINode.h>
#include <dukexcore/dkxSessionDescriptor.h>
#include <dukeapi/protobuf_builder/CmdLineParser.h>
#include <dukeapi/protobuf_builder/SceneBuilder.h>

#include <player.pb.h>

#include <boost/filesystem.hpp>
#include <deque>

class PlaylistNode : public INode {

public:
    typedef boost::shared_ptr<PlaylistNode> ptr;
    PlaylistNode() :
                    INode("fr.mikrosimage.dukex.playlist") {
    }

public:
    bool openFiles(const std::vector<std::string> inputs, const bool browseMode = false) {
        try {
            SessionDescriptor & descriptor = session()->descriptor();

            // Save current playlist parameters
            ::duke::protocol::Scene & scene = descriptor.scene();
            ::google::protobuf::uint32 framerate = scene.frameratenumerator();
            ::duke::protocol::Scene_PlaybackMode playbackmode = scene.playbackmode();

            // Clear current playlist
            scene.Clear();

            // Parse all inputs
            MessageQueue queue;
            IOQueueInserter inserter(queue);

            // Push engine stop
            ::duke::protocol::Engine stop;
            stop.set_action(::duke::protocol::Engine::RENDER_STOP);
            inserter << stop;

            const extension_set validExtensions = extension_set::create(session()->getAvailableExtensions());
            duke::playlist::Playlist playlist = browseMode ? browseViewerComplete(validExtensions, inputs[0]) : browsePlayer(validExtensions, inputs);
            playlist.set_framerate(framerate);
            normalize(playlist);
            std::vector<google::protobuf::serialize::SharedHolder> messages = getMessages(playlist, playbackmode);
            queue.drainFrom(messages);

            if (playlist.has_startframe()) {
                duke::protocol::Transport cue;
                cue.set_type(duke::protocol::Transport::CUE);
                cue.mutable_cue()->set_value(playlist.startframe());
                push(queue, cue);
            }

            // Push engine start
            ::duke::protocol::Engine start;
            start.set_action(::duke::protocol::Engine::RENDER_START);
            inserter << start;

            // send everything
            session()->sendMsg(queue);

        } catch (std::exception & e) {
            std::cerr << "[PlaylistNode] " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool setFramerate(float framerate) {
        try {
            SessionDescriptor & descriptor = session()->descriptor();
            ::duke::protocol::Scene & p = descriptor.scene();
            p.set_frameratenumerator((int) framerate);
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
//            MessageQueue queue;
//            ::duke::protocol::Debug d;
//            d.add_line("%0");
//            d.add_line("%1");
//            d.add_line("%2");
//            d.add_content(::duke::protocol::Debug_Content_FRAME);
//            d.add_content(::duke::protocol::Debug_Content_FILENAMES);
//            d.add_content(::duke::protocol::Debug_Content_FPS);
//            push(queue, d, google::protobuf::serialize::MessageHolder::RETRIEVE);
//            session()->sendMsg(queue);

            MessageQueue queue;
            ::duke::protocol::Info info;
            info.set_content(::duke::protocol::Info_Content_IMAGEINFO);
            push(queue, info, google::protobuf::serialize::MessageHolder::RETRIEVE);
            session()->sendMsg(queue);

        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
};

#endif // PLAYLISTNODE_H
