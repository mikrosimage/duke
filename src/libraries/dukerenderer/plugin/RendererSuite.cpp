#include "RendererSuite.h"

using namespace ::google::protobuf;
using namespace ::protocol::duke;
using namespace ::std;

RendererSuite::RendererSuite(const OfxHost* pHost, const OfxRendererSuiteV1* pSuite) :
    m_pHost(pHost), m_pSuite(pSuite) {
}

#include <iostream>
void RendererSuite::renderStart() const {
    assert(m_pSuite->renderStart);
    (*m_pSuite->renderStart)();
}

OfxRendererSuiteV1::PresentStatus RendererSuite::getPresentStatus() const {
    assert(m_pSuite->getPresentStatus);
    return (*m_pSuite->getPresentStatus)();
}

void RendererSuite::verticalBlanking(bool presented) const {
    assert(m_pSuite->verticalBlanking);
    (*m_pSuite->verticalBlanking)(presented);
}

bool RendererSuite::renderEnd(unsigned msToPresent) const {
    assert(m_pSuite->renderEnd);
    return (*m_pSuite->renderEnd)(msToPresent);
}

const Setup& RendererSuite::getSetup() const {
    assert(m_pSuite->pushEvent);
    return m_pSuite->m_Setup;
}

void RendererSuite::pushEvent(unique_ptr<Message>& msg) const {
    assert(m_pSuite->pushEvent);
    (*m_pSuite->pushEvent)(msg);
}

const Message* RendererSuite::popEvent(MessageType& type) const {
    assert(m_pSuite->popEvent);
    return (*m_pSuite->popEvent)(type);
}

