#ifndef GRADINGNODE_H
#define GRADINGNODE_H

#include <dukexcore/dkxINode.h>
#include <dukexcore/dkxSessionDescriptor.h>
#include <dukeapi/messageBuilder/ParameterBuilder.h>
#include <dukeapi/protobuf_builder/SceneBuilder.h>
#include <player.pb.h>


class GradingNode : public INode {

public:
    typedef boost::shared_ptr<GradingNode> ptr;
    GradingNode() :
        INode("fr.mikrosimage.dukex.grading") {
    }
    virtual ~GradingNode() {
    }

public:
    bool setZoom(double zoomRatio) {
        try {
            MessageQueue queue;
            addStaticFloatParam(queue, "zoom", zoomRatio);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool setPan(double panX, double panY) {
        try {
            MessageQueue queue;
            addStaticFloatParam(queue, "panX", panX);
            addStaticFloatParam(queue, "panY", panY);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool setColorspace(::duke::playlist::Display::ColorSpace colorspace) {
        try {
            MessageQueue queue;
            IOQueueInserter inserter(queue);

            // Push engine stop
            ::duke::protocol::Engine stop;
            stop.set_action(::duke::protocol::Engine::RENDER_STOP);
            inserter << stop;

            // Get playlist from session
            ::duke::playlist::Playlist & playlist = session()->descriptor().playlist();
            // Set the right colorspace
            if(playlist.has_display()){
                playlist.mutable_display()->set_colorspace(colorspace);
            }
            normalize(playlist);
            session()->descriptor().setPlaylist(playlist);
            std::vector<google::protobuf::serialize::SharedHolder> messages = getMessages(playlist);
            queue.drainFrom(messages);

            // Push engine start
            ::duke::protocol::Engine start;
            start.set_action(::duke::protocol::Engine::RENDER_START);
            inserter << start;

            // Send
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

};

#endif // GRADINGNODE_H
