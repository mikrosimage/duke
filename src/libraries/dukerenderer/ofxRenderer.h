/*
 * ofxRenderer.h
 *
 *  Created on: 13 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef OFXRENDERER_H_
#define OFXRENDERER_H_

#include "Setup.h"

#include <openfx/ofxCorePlugin.h>

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
    void (* pushEvent)(std::unique_ptr<google::protobuf::Message>&);
    const ::google::protobuf::Message* (*popEvent)(::protocol::duke::MessageType&);
    bool (* renderEnd)(unsigned msToPresent);

    Setup m_Setup;

    OfxRendererSuiteV1() :
        renderStart(NULL), getPresentStatus(NULL), verticalBlanking(NULL), pushEvent(NULL), popEvent(NULL), renderEnd(NULL) {
    }

} OfxRendererSuiteV1;

#define kOfxActionRendererInit          "Renderer"
#define kOfxActionWaitForMainLoopEnd    "waitForMainLoopEnd"

#endif /* OFXRENDERER_H_ */
