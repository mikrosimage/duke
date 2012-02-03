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

#define FITMODESIZE 3

} // namespace

class FitNode : public INode {

public:
    typedef boost::shared_ptr<FitNode> ptr;

public:
    FitNode() :
        INode("fr.mikrosimage.dukex.fit") {
    }

public:
    void toggle(int _mode = -1) {
        SessionDescriptor & descriptor = session()->descriptor();
        ::duke::protocol::StaticParameter & p = descriptor.displayMode();
        if(_mode < 0 || _mode >= FITMODESIZE){
            if (p.floatvalue_size() > 0){ // current +1
                _mode = (int)p.floatvalue(0);
                _mode = (_mode + 1) % FITMODESIZE;
            } else { // default +1
                _mode = 1;
            }
        }
        MessageQueue q;
        push(q, MAKE(p, _mode));
        addStaticFloatParam(q, "panX", 0.);
        addStaticFloatParam(q, "panY", 0.);
        addStaticFloatParam(q, "zoom", 1.);
        session()->sendMsg(q);
    }

    void fitToNormalSize() {
        toggle(0);
    }

    void fitImageToWindowHeight() {
        toggle(1);
    }

    void fitImageToWindowWidth() {
        toggle(2);
    }

    void stretchImageToWindow() {
        toggle(3);
    }
};

#endif // FITNODE_H
