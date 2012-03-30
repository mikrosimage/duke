#ifndef SCENENODE_H
#define SCENENODE_H

#include <dukexcore/dkxINode.h>
#include <dukexcore/dkxSessionDescriptor.h>
#include <dukeapi/protobuf_builder/CmdLineParser.h>
#include <dukeapi/protobuf_builder/SceneBuilder.h>
#include <player.pb.h>
#include <boost/filesystem.hpp>
#include <deque>

class SceneNode : public INode {

public:
    typedef boost::shared_ptr<SceneNode> ptr;
    SceneNode() :
        INode("fr.mikrosimage.dukex.scene") {
    }

public:
    bool openFiles(const std::vector<std::string> inputs, const bool browseMode = false) {
        try {
            MessageQueue queue;
            IOQueueInserter inserter(queue);

            // retrieve playlist parameters
            ::google::protobuf::uint32 framerate = session()->descriptor().framerate();
            ::duke::protocol::Scene_PlaybackMode playbackmode = session()->descriptor().playbackMode();

            // Push engine stop
            ::duke::protocol::Engine stop;
            stop.set_action(::duke::protocol::Engine::RENDER_STOP);
            inserter << stop;

            // Build the new playlist
            const extension_set validExtensions = extension_set::create(session()->factory().getAvailableExtensions());
            duke::playlist::Playlist playlist = browseMode ? browseViewerComplete(validExtensions, inputs[0]) : browsePlayer(validExtensions, inputs);
            playlist.set_framerate(framerate);
            normalize(playlist);

            // Register in session
            session()->descriptor().setPlaylist(playlist);

            // Build messages from playlist
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

    void registerFramerate(float framerate) {
        SessionDescriptor & descriptor = session()->descriptor();
        descriptor.setFramerate(framerate);
    }

    bool setFramerate(float framerate) {
        try {
            // register new framerate value
            registerFramerate(framerate);

            // re-build & push the entire scene
            MessageQueue queue;
            SessionDescriptor & descriptor = session()->descriptor();
            const ::duke::playlist::Playlist & p = descriptor.playlist();

            std::vector<google::protobuf::serialize::SharedHolder> messages = getMessages(p, descriptor.playbackMode());
            queue.drainFrom(messages);

            duke::protocol::Transport cue;
            cue.set_type(duke::protocol::Transport::CUE);
            cue.mutable_cue()->set_value(descriptor.currentFrame());
            push(queue, cue);

            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
};

#endif // SCENENODE_H
