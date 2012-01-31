#ifndef SHADINGNODE_H
#define SHADINGNODE_H

#include <dukexcore/dkxINode.h>

class ShadingNode : public INode {

public:
    typedef boost::shared_ptr<ShadingNode> ptr;

public:
    ShadingNode() :
        INode("fr.mikrosimage.dukex.shading") {
    }

public:
    void setGamma(const float _f) {

//        StaticParameter g;
//        g.set_name("invgamma");
//        g.set_type(StaticParameter_Type_FLOAT);
//        g.add_floatvalue(1.f / _f);
//        dynamic_cast<IMessageOutput&>(m_PendingMessageQueue).push(g);
    }
};

#endif // SHADINGNODE_H
