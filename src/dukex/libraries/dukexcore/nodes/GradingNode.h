#ifndef GRADINGNODE_H
#define GRADINGNODE_H

#include <dukexcore/dkxINode.h>
#include <dukexcore/dkxSessionDescriptor.h>
#include <dukeapi/core/messageBuilder/ParameterBuilder.h>
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

};

#endif // GRADINGNODE_H
