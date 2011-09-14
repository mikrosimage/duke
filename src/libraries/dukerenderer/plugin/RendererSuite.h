#ifndef RENDERERSUITE_H_
#define RENDERERSUITE_H_

#include "ofxRenderer.h"
#include <dukeapi/protocol/player/protocol.pb.h>

class RendererSuite {
    const OfxHost* m_pHost;
    const OfxRendererSuiteV1* m_pSuite;

public:
    RendererSuite(const OfxHost* pHost, const OfxRendererSuiteV1* pSuite);

    void renderStart() const;

    OfxRendererSuiteV1::PresentStatus getPresentStatus() const;

    void verticalBlanking(bool presented) const;

    void pushEvent(std::unique_ptr<google::protobuf::Message>&) const;

    const ::google::protobuf::Message* popEvent(::duke::protocol::MessageType&) const;

    bool renderEnd(unsigned msToPresent) const;

    const Setup& getSetup() const;
};

#endif /* RENDERERSUITE_H_ */
