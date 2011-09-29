#ifndef RENDERERSUITE_H_
#define RENDERERSUITE_H_

#include "ofxRenderer.h"
#include <player.pb.h>

class RendererSuite {
    const OfxHost* m_pHost;
    const OfxRendererSuiteV1* m_pSuite;

public:
    RendererSuite(const OfxHost* pHost, const OfxRendererSuiteV1* pSuite);

    void renderStart() const;

    OfxRendererSuiteV1::PresentStatus getPresentStatus() const;

    void verticalBlanking(bool presented) const;

    void pushEvent(const google::protobuf::serialize::MessageHolder&) const;

    const google::protobuf::serialize::MessageHolder * const  popEvent() const;

    bool renderEnd(unsigned msToPresent) const;

    const Setup& getSetup() const;
};

#endif /* RENDERERSUITE_H_ */
