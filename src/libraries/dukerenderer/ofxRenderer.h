#ifndef OFXRENDERER_H_
#define OFXRENDERER_H_

#include "Setup.h"

#include <openfx/ofxCorePlugin.h>
#include <protocol.pb.h>

#include <memory>

#define kOfxRendererPluginApi "MikrosImage player renderer"
#define kOfxRendererSuite "RendererSuite"

typedef struct OfxRendererSuiteV1 {
    enum PresentStatus {
        PRESENT_IMMEDIATE = 0, PRESENT_NEXT_BLANKING = 1, SKIP_NEXT_BLANKING = 2
    };
    void (* renderStart)();
    PresentStatus (* getPresentStatus)();
    void (* verticalBlanking)(bool verticalBlanking);
    void (* pushEvent)(const google::protobuf::serialize::MessageHolder&);
    const google::protobuf::serialize::MessageHolder * const (*popEvent)();
    bool (* renderEnd)(unsigned msToPresent);

    Setup m_Setup;

    OfxRendererSuiteV1() :
        renderStart(NULL), getPresentStatus(NULL), verticalBlanking(NULL), pushEvent(NULL), popEvent(NULL), renderEnd(NULL) {
    }

} OfxRendererSuiteV1;

#define kOfxActionRendererInit          "Renderer"
#define kOfxActionWaitForMainLoopEnd    "waitForMainLoopEnd"

#endif /* OFXRENDERER_H_ */
