/*
 * Application.h
 *
 *  Created on: 13 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "audio/AudioEngine.h"
#include "file/FileBufferHolder.h"
#include "playback/Playback.h"
#include "time_statistics/Durations.h"
//#include "time/FramePresenter.h"
#include "playback/Timings.h"
#include "sequence/PlaylistHelper.h"
#include "image/ImageCache.h"
#include "image/ImageReader.h"
#include "host/renderer/Renderer.h"
#include "host/io/ImageDecoderFactory.h"
#include <dukeapi/core/MessageHolder.h>
#include <dukeapi/core/queue/MessageQueue.h>
#include <dukerenderer/ofxRenderer.h>

#include <boost/shared_ptr.hpp>

// fwd declaration
class Renderer;
class PlaylistHelper;
namespace google {
namespace protobuf {
class Message;
}
}

class Application {
public:
    Application(const char* rendererFilename, IMessageIO &IO, int &returnCode, const size_t cacheSize = 0);
    ~Application();

    void* fetchSuite(const char* suiteName, int suiteVersion);
    void renderStart();
    void verticalBlanking(bool presented);
    bool renderFinished(unsigned msToPresent);
    OfxRendererSuiteV1::PresentStatus getPresentStatus();
    void pushEvent(std::unique_ptr<google::protobuf::Message>& event);
    const ::google::protobuf::Message* popEvent(::protocol::duke::MessageType& type);

private:
    void applyTransport(const ::protocol::duke::Transport&);
    void consumeUntilRenderOrQuit();
    void consumeTransport();
    void handleQuitMessage(const ::protocol::duke::Quit&) const;
    std::string dumpInfo(const ::protocol::duke::Debug_Content& debug) const;

private:
    // order of variables are very important because of multi threading issues
    IMessageIO &m_IO;
    MessageQueue m_RendererMessages;
    SharedMessage m_RendererMessageHolder;
    ImageDecoderFactory m_ImageDecoderFactory;
    ImageReader m_ImageReader;
    PlaylistHelper m_Playlist;
    playback::PlaybackState m_Playback;
    FileBufferHolder m_FileBufferHolder;
    Timings m_VbiTimings;
    Timings m_FrameTimings;
    Durations m_Durations;
    //    AudioEngine m_AudioEngine;
    uint32_t m_PreviousFrame;
    uint32_t m_StoredFrame;
    bool m_bRequestTermination;
    bool m_bAutoNotifyOnFrameChange;
    int &m_iReturnCode;
    // all the following objects are to be initialized in this order because of the dependencies between them
    // they also have to be initialized after the previous ones
    Renderer m_Renderer;
};

#endif /* APPLICATION_H_ */
