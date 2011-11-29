#include "Application.h"

#include "host/renderer/Renderer.h"
#include "range/PlaylistRange.h"

#include <player.pb.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/locks.hpp>
#include <boost/chrono.hpp>

#include <iostream>
#include <sstream>
#include <cassert>
#include <set>

using namespace ::google::protobuf::serialize;
using namespace ::google::protobuf;
using namespace ::duke::protocol;
using namespace ::std;

const string HEADER = "[Application] ";

namespace {

Application* g_pApplication;

void renderStart() {
    g_pApplication->renderStart();
}

void pushEvent(const google::protobuf::serialize::MessageHolder&event) {
    g_pApplication->pushEvent(event);
}

const google::protobuf::serialize::MessageHolder * popEvent() {
    return g_pApplication->popEvent();
}

bool renderFinished(unsigned msToPresent) {
    return g_pApplication->renderFinished(msToPresent);
}

void verticalBlanking(bool presented) {
    return g_pApplication->verticalBlanking(presented);
}

OfxRendererSuiteV1::PresentStatus getPresentStatus() {
    return g_pApplication->getPresentStatus();
}

void* fetchSuite(OfxPropertySetHandle host, const char* suiteName, int suiteVersion) {
    return ((Application*) host)->fetchSuite(suiteName, suiteVersion);
}

OfxRendererSuiteV1 buildRendererSuite() {
    OfxRendererSuiteV1 rendererSuite;
    rendererSuite.verticalBlanking = &::verticalBlanking;
    rendererSuite.getPresentStatus = &::getPresentStatus;
    rendererSuite.renderStart = &::renderStart;
    rendererSuite.renderEnd = &::renderFinished;
    rendererSuite.pushEvent = &::pushEvent;
    rendererSuite.popEvent = &::popEvent;
    return rendererSuite;
}

OfxRendererSuiteV1 g_ApplicationRendererSuite = buildRendererSuite();

OfxHost buildHost(Application* pApplication) {
    g_pApplication = pApplication;
    OfxHost ofxHost;
    ofxHost.host = (OfxPropertySetHandle) pApplication;
    ofxHost.fetchSuite = &::fetchSuite;
    return ofxHost;
}

} // namespace

static inline void dump(const google::protobuf::Descriptor* pDescriptor, const google::protobuf::serialize::MessageHolder &holder, bool push = false) {
#ifdef DEBUG_MESSAGES
    const string debugString = pDescriptor == Texture::descriptor() ? "texture" : unpack(holder)->ShortDebugString();
    cerr << HEADER + (push ? "push " : "pop  ") + pDescriptor->name() << "\t" << debugString << endl;
#endif
}

static inline playback::PlaybackState create(const PlaylistHelper &helper) {
    const Playlist &playlist = helper.getPlaylist();
    const playback::duration nsPerFrame = playback::nsPerFrame(playlist.frameratenumerator(), playlist.frameratedenominator());
    cout << HEADER << nsPerFrame << endl;
    return playback::PlaybackState(nsPerFrame, helper.getFirstFrame(), helper.getLastFrame(), playlist.loop());
}

static uint32_t getFrameFromCueMessage(const Transport_Cue& cue, const PlaylistHelper &helper, uint32_t newFrame) {
    const bool isCueClip = cue.cueclip();
    const bool isCueRelative = cue.cuerelative();
    const int32_t value = cue.value();

    const Playlist &p(helper.getPlaylist());
    const bool loop = p.loop();
    if (isCueClip) { // cueing clips
        const int clipSize = p.clip_size();
        if (isCueRelative) { // cueing clips relative
            if (clipSize == 0)
                return newFrame;
            std::set<size_t> clips;
            for (int i = 0; i < clipSize; ++i)
                clips.insert(p.clip(i).recin());
            std::set<size_t>::const_iterator it = clips.lower_bound(newFrame);
            std::advance(it, value);
            if (it == clips.end()) {
                if (loop) {
                    const int32_t clipIndex = value < 0 ? clipSize - 1 : 0;
                    newFrame = p.clip(clipIndex).recin();
                }
            } else {
                newFrame = *it;
            }
        } else { // cueing clips absolute
            if ((value < clipSize) && (value >= 0))
                newFrame = p.clip(value).recin();
        }
    } else { // cueing frames
        if (isCueRelative)
            newFrame += value;
        else
            newFrame = value;
        if (!loop && newFrame < 0)
            newFrame = helper.getFirstFrame();
    }

    if (loop && newFrame < 0)
        newFrame = helper.getLastFrame() + newFrame;

    return newFrame;
}

Application::Application(const char* rendererFilename, IMessageIO &io, int &returnCode, const uint64_t cacheSize, const size_t cacheThreads) :
        m_IO(io), //
        m_AudioEngine(), //
        m_Cache(cacheThreads, cacheSize, m_ImageDecoderFactory), //
        m_FileBufferHolder(), //
        m_VbiTimings(VBI, 120), //
        m_FrameTimings(FRAME, 10), //
        m_PreviousFrame(-1), //
        m_StoredFrame(-1), //
        m_bRequestTermination(false), //
        m_bAutoNotifyOnFrameChange(false), //
        m_iReturnCode(returnCode), //
        m_Renderer(buildHost(this), rendererFilename) {

    consumeUntilRenderOrQuit();
}

Application::~Application() {
    g_pApplication = NULL;
}

void Application::consumeUntilRenderOrQuit() {
    using ::duke::protocol::Renderer;
    SharedHolder pHolder;
    while (true) {
        m_IO.waitPop(pHolder);

        if (handleQuitMessage(pHolder))
            return;

        const MessageHolder &holder = *pHolder;
        const Descriptor* descriptor = descriptorFor(holder);

        if (isType<Renderer>(descriptor)) {
            m_Renderer.initRender(unpackTo<Renderer>(holder));
            break;
        }
        cerr << HEADER + "First message must be either InitRenderer or Quit, ignoring message of type " << descriptor->name() << endl;
    }
}

bool Application::handleQuitMessage(const ::google::protobuf::serialize::SharedHolder& pHolder) {
    if (!pHolder) {
        m_iReturnCode = EXIT_FAILURE;
        return true;
    }
    if (pHolder->action() == MessageHolder_Action_CLOSE_CONNECTION) {
        m_iReturnCode = pHolder->return_value();
        return true;
    }
    return false;
}

void* Application::fetchSuite(const char* suiteName, int suiteVersion) {
    return &g_ApplicationRendererSuite;
}

void Application::applyTransport(const Transport& transport) {
    //                        m_AudioEngine.applyTransport(transport);
    const uint32_t currentFrame = m_Playback.frame();
    switch (transport.type()) {
        case Transport_TransportType_PLAY:
            m_Playback.play(currentFrame, 1);
            m_AudioEngine.sync(m_Playback.playlistTime());
            m_AudioEngine.play();
            break;
        case Transport_TransportType_STOP:
            m_Playback.cue(currentFrame);
            m_AudioEngine.pause();
            break;
        case Transport_TransportType_STORE:
            m_StoredFrame = currentFrame;
            break;
        case Transport_TransportType_CUE:
            m_Playback.cue(getFrameFromCueMessage(transport.cue(), m_Playlist, currentFrame));
            m_AudioEngine.pause();
            break;
        case Transport_TransportType_CUE_FIRST:
            m_Playback.cue(m_Playlist.getFirstFrame());
            m_AudioEngine.pause();
            break;
        case Transport_TransportType_CUE_LAST:
            m_Playback.cue(m_Playlist.getLastFrame());
            m_AudioEngine.pause();
            break;
        case Transport_TransportType_CUE_STORED:
            m_Playback.cue(m_StoredFrame);
            m_AudioEngine.pause();
            break;
    }
}

void Application::consumeTransport() {
    using ::duke::protocol::Renderer;

    SharedHolder pHolder;
    while (m_IO.tryPop(pHolder)) {
        if (handleQuitMessage(pHolder)) {
            m_bRequestTermination = true;
            cerr << "handling Quit message and quitting" << endl;
            return;
        }
        const MessageHolder &holder = *pHolder;
        const Descriptor* pDescriptor = descriptorFor(holder);
        if (isType<Renderer>(pDescriptor)) {
            cerr << HEADER + "calling INIT_RENDERER twice is forbidden" << endl;
        } else if (isType<Debug>(pDescriptor)) {
            dump(pDescriptor, holder);
            const Debug debug = unpackTo<Debug>(holder);
#ifdef __linux__
            std::cout << "\e[J";
#endif
            for (int i = 0; i < debug.line_size(); ++i) {
                size_t found;
                std::string line = debug.line(i);
                found = line.find_first_of("%");

                while (found != string::npos) {
                    std::stringstream ss;
                    ss << line[found + 1];
                    int contentID = atoi(ss.str().c_str());
                    if (contentID < debug.content_size())
                        line.replace(found, 2, dumpInfo(debug.content(contentID)));
                    found = line.find_first_of("%", found + 1);
                }
                std::cout << line << std::endl;
            }
#ifdef __linux__
            std::stringstream ss;
            ss << "\r\e[" << debug.line_size() + 1 << "A";
            std::cout << ss.str() << std::endl;
#endif
            if (debug.has_pause())
                ::boost::this_thread::sleep(::boost::posix_time::seconds(debug.pause()));
        } else if (isType<Playlist>(pDescriptor)) {
            dump(pDescriptor, holder);
            PlaylistHelper temporary(unpackTo<Playlist>(holder));
            m_Playlist.swap(temporary);
            m_AudioEngine.load(unpackTo<Playlist>(holder));
            m_Playback = create(m_Playlist);
        } else if (isType<Transport>(pDescriptor)) {
            dump(pDescriptor, holder);
            switch (pHolder->action()) {
                case MessageHolder_Action_CREATE: {
                    const Transport transport = unpackTo<Transport>(holder);
                    applyTransport(transport);
                    if (transport.has_autonotifyonframechange())
                        m_bAutoNotifyOnFrameChange = transport.autonotifyonframechange();
                    if (transport.has_dorender() && transport.dorender())
                        return;
                    break;
                }
                case MessageHolder_Action_RETRIEVE: {
                    Transport transport;
                    transport.set_type(::Transport_TransportType_CUE);
                    Transport_Cue *cue = transport.mutable_cue();
                    cue->set_value(m_Playback.frame());
                    push(m_IO, transport);
                    break;
                }
                default: {
                    cerr << HEADER + "unknown action for transport message " << MessageHolder_Action_Name(holder.action()) << endl;
                    break;
                }
            }
        } else {
            m_RendererMessages.push(pHolder);
        }
    }
}

void Application::renderStart() {
    try {
        // consume message
        consumeTransport();

        // update current frame
        if (m_Playback.adjustCurrentFrame())
            cout << "unstable" << endl;
        const size_t frame = m_Playback.frame();

        // sync audio
        if (m_Playback.isPlaying())
            m_AudioEngine.checksync(m_Playback.playlistTime());

        // retrieve images
        Setup &setup(g_ApplicationRendererSuite.m_Setup);
        setup.m_Images.clear();
        if (m_PreviousFrame != frame && m_Playlist.getEndIterator() != 0) {
            m_Cache.seek(frame, m_Playback.getSpeed(), m_Playlist);
            m_FileBufferHolder.update(frame, m_Cache, m_Playlist);
        }

        BOOST_FOREACH( const ImageHolder &image, m_FileBufferHolder.getImages() )
                {
                    setup.m_Images.push_back(image.getImageDescription());
                }

        // populate clips
        m_Playlist.getClipsAtFrame(frame, setup.m_Clips);

        // set current frame
        setup.m_iFrame = frame;
    } catch (exception& e) {
        cerr << HEADER + "Unexpected error while starting simulation step : " << e.what() << endl;
    }
}

OfxRendererSuiteV1::PresentStatus Application::getPresentStatus() {
    return m_Playback.shouldPresent() ? OfxRendererSuiteV1::PRESENT_NEXT_BLANKING : OfxRendererSuiteV1::SKIP_NEXT_BLANKING;
}

void Application::verticalBlanking(bool presented) {
    const boost::chrono::high_resolution_clock::time_point now(playback::s_Clock.now());
    m_VbiTimings.push(now);
    if (presented)
        m_FrameTimings.push(now);
}

bool Application::renderFinished(unsigned msToPresent) {
    try {
        const uint32_t newFrame = m_Playback.frame();
        if (m_PreviousFrame != newFrame && m_bAutoNotifyOnFrameChange) {
            Transport transport;
            transport.set_type(::Transport_TransportType_CUE);
            Transport_Cue *cue = transport.mutable_cue();
            cue->set_value(newFrame);
            push(m_IO, transport);
        }
        //        if(m_PlaybackState.isLastFrame() || newFrame < m_PreviousFrame){
        //            m_AudioEngine.rewind();
        //        }
        m_PreviousFrame = newFrame;
        // cout << round(m_FrameTimings.frequency()) << "FPS";
        return m_bRequestTermination;
    } catch (exception& e) {
        cerr << HEADER + "Unexpected error while finishing simulation step : " << e.what() << endl;
    }
    return true;
}

void Application::pushEvent(const google::protobuf::serialize::MessageHolder& event) {
    dump(descriptorFor(event), event, true);
    m_IO.push(makeSharedHolder(event));
}

const google::protobuf::serialize::MessageHolder * Application::popEvent() {
    m_RendererMessages.tryPop(m_RendererMessageHolder);
    return m_RendererMessageHolder.get();
}

std::string Application::dumpInfo(const Debug_Content& info) const {
    std::stringstream ss;

    switch (info) {
        case Debug_Content_FRAME:
            ss << m_Playback.frame();
            break;
        case Debug_Content_FILENAMES: {
            std::vector<size_t> indices;
            m_Playlist.getIteratorsAtFrame(m_Playback.frame(), indices);

            BOOST_FOREACH(size_t i, indices)
                    {
                        ss << m_Playlist.getPathAtIterator(i).filename() << " ";
                    }
            break;
        }
        case Debug_Content_FPS: {
            ss << m_FrameTimings.frequency();
            break;
        }
    }
    return ss.str();
}

