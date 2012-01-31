#ifndef FITNODE_H
#define FITNODE_H

#include <dukexcore/dkxINode.h>
#include <dukeapi/core/messageBuilder/ParameterBuilder.h>

namespace {

::duke::protocol::StaticParameter & MAKE(::duke::protocol::StaticParameter & p, const float _f) {
    p.set_name("displayMode");
    p.set_type(::duke::protocol::StaticParameter_Type_FLOAT);
    if (p.floatvalue_size() == 0)
        p.add_floatvalue(_f);
    else
        p.set_floatvalue(0, _f);
    return p;
}

} // namespace

class FitNode : public INode {

public:
    typedef boost::shared_ptr<FitNode> ptr;

public:
    FitNode() :
        INode("fr.mikrosimage.dukex.fit") {
    }

public:
    void toggle() {
        SessionDescriptor & descriptor = session()->descriptor();
        ::duke::protocol::StaticParameter & p = descriptor.displayMode();
        int mode = 0;
        if (p.floatvalue_size() > 0)
            mode = (int) p.floatvalue(0);
        mode = (mode + 1) % 4;
        MessageQueue q;
        push(q, MAKE(p, mode));
        addStaticFloatParam(q, "panX", 0.);
        addStaticFloatParam(q, "panY", 0.);
        addStaticFloatParam(q, "zoom", 1.);
        session()->sendMsg(q);
    }

    void fitToNormalSize() {
        SessionDescriptor & descriptor = session()->descriptor();
        MessageQueue q;
        push(q, MAKE(descriptor.displayMode(), 0.f));
        session()->sendMsg(q);
    }

    void fitImageToWindowHeight() {
        SessionDescriptor & descriptor = session()->descriptor();
        MessageQueue q;
        push(q, MAKE(descriptor.displayMode(), 1.f));
        session()->sendMsg(q);
    }

    void fitImageToWindowWidth() {
        SessionDescriptor & descriptor = session()->descriptor();
        MessageQueue q;
        push(q, MAKE(descriptor.displayMode(), 2.f));
        session()->sendMsg(q);
    }

    void stretchImageToWindow() {
        SessionDescriptor & descriptor = session()->descriptor();
        MessageQueue q;
        push(q, MAKE(descriptor.displayMode(), 3.f));
        session()->sendMsg(q);
    }
};

#endif // FITNODE_H
