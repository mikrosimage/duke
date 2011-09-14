#include "Application.h"
#include "host/renderer/Renderer.h"
#include <dukeapi/protocol/player/protocol.pb.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/locks.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <sstream>
#include <cassert>
#include <set>

using namespace ::google::protobuf;
using namespace ::duke::protocol;
using namespace ::std;

const string HEADER = "[Application] ";

namespace {

Application* g_pApplication;

void renderStart() {
    g_pApplication->renderStart();
}

void pushEvent(unique_ptr<Message>& event) {
    g_pApplication->pushEvent(event);
}

const Message* popEvent(::MessageType& type) {
    return g_pApplication->popEvent(type);
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


static void dump(const SharedMessage msg) {
#ifdef DEBUG_MESSAGES
    cerr << HEADER + "pop " + MessageType_Type_Name(msg->type()) << "\t" << msg->message().ShortDebugString() << endl;
#endif
}

Application::Application(const char* rendererFilename, IMessageIO &io, int &returnCode, const size_t cacheSize) :
    m_IO(io), //
            m_ImageReader(m_ImageDecoderFactory), //
            // m_AudioEngine(AudioEngine::CurrentVideoFrameCallback(boost::bind(&PlaybackState::getCurrentFrame, &m_PlaybackState))) ,//
            m_FileBufferHolder(m_ImageReader), //
            m_VbiTimings(TimingType::VBI, 120), //
            m_FrameTimings(TimingType::FRAME, 10), //
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
    cout << HEADER + "engine stopped" << endl;
}

void Application::consumeUntilRenderOrQuit() {
    SharedMessage holder;
    m_IO.waitPop(holder);
    assert(holder);
    const MessageType_Type type(holder->type());
    if (type == MessageType_Type_INIT_RENDERER)
        m_Renderer.initRender(holder->message<duke::protocol::Renderer> ());
    else if (type == MessageType_Type_QUIT)
        handleQuitMessage(holder->message<Quit> ());
    else
        cerr << HEADER + "First message must be either InitRenderer or Quit, was " << MessageType_Type_Name(holder->type()) << endl;
}

void Application::handleQuitMessage(const Quit& quit) const {
    for (int i = 0; i < quit.quitmessage_size(); ++i)
        cout << quit.quitmessage(i) << endl;
    m_iReturnCode = quit.returncode();
}

void* Application::fetchSuite(const char* suiteName, int suiteVersion) {
    return &g_ApplicationRendererSuite;
}

static inline playback::PlaybackState create(const PlaylistHelper &helper) {
    const auto &playlist = helper.getPlaylist();
    const playback::duration nsPerFrame = playback::nsPerFrame(playlist.frameratenumerator(), playlist.frameratedenominator());
    cout << HEADER << nsPerFrame << endl;
    return playback::PlaybackState(nsPerFrame, helper.getFirstFrame(), helper.getLastFrame(), playlist.loop());
}

void Application::consumeTransport() {
    SharedMessage holder;
    while (m_IO.tryPop(holder)) {
        switch (holder->type()) {
            case MessageType_Type_INIT_RENDERER:
                cerr << HEADER + "calling INIT_RENDERER twice is forbidden" << endl;
                break;
            case MessageType_Type_DEBUG_MSG: {
                dump(holder);
                const Debug& debug = holder->message<Debug> ();
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
                ss << "\r\e[" << debug.line_size()+1 << "A";
                std::cout << ss.str() << std::endl;
#endif
                if (debug.has_pause())
                    ::boost::this_thread::sleep(::boost::posix_time::seconds(debug.pause()));
                break;
            }
            case MessageType_Type_INIT_PLAYLIST: {
                dump(holder);
                m_Playlist.swap(PlaylistHelper(holder->message<Playlist> ()));
                m_Playback = create(m_Playlist);
                //                m_AudioEngine.load(p);
                break;
            }
            case MessageType_Type_QUIT: {
                dump(holder);
                m_bRequestTermination = true;
                handleQuitMessage(holder->message<Quit> ());
                return;
            }
            case MessageType_Type_TRANSPORT:
                dump(holder);
                switch (holder->operation()) {
                    case MessageType_Action_SET: {
                        const Transport transport = holder->message<Transport> ();
                        applyTransport(transport);
                        if (transport.has_autonotifyonframechange())
                            m_bAutoNotifyOnFrameChange = transport.autonotifyonframechange();
                        if (transport.has_dorender() && transport.dorender())
                            return;
                        break;
                    }
                    case MessageType_Action_RETRIEVE: {
                        Transport transport;
                        transport.set_type(::Transport_TransportType_CUE);
                        Transport_Cue *cue = transport.mutable_cue();
                        cue->set_value(m_Playback.frame());
                        push(m_IO, transport);
                        break;
                    }
                    default:
                        cerr << HEADER + "unknown action for transport message " << MessageType_Action_Name(holder->operation()) << endl;
                }
                break;
            default:
                m_RendererMessages.push(holder);
        }
    }
}

static uint32_t getFrameFromCueMessage(const Transport_Cue& cue, const PlaylistHelper &helper, uint32_t newFrame) {
    const bool isCueClip = cue.cueclip();
    const bool isCueRelative = cue.cuerelative();
    const auto value = cue.value();

    const Playlist &p(helper.getPlaylist());
    const auto loop = p.loop();
    if (isCueClip) { // cueing clips
        const auto clipSize(p.clip_size());
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
                    const auto clipIndex = value < 0 ? clipSize - 1 : 0;
                    newFrame = p.clip(clipIndex).recin();
                }
            } else {
                newFrame = *it;
            }
        } else {// cueing clips absolute
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

void Application::applyTransport(const Transport& transport) {
    //                        m_AudioEngine.applyTransport(transport);
    const uint32_t currentFrame = m_Playback.frame();
    switch (transport.type()) {
        case Transport_TransportType_PLAY:
            m_Playback.play(currentFrame, 1);
            break;
        case Transport_TransportType_STOP:
            m_Playback.cue(currentFrame);
            break;
        case Transport_TransportType_STORE:
            m_StoredFrame = currentFrame;
            break;
        case Transport_TransportType_CUE:
            m_Playback.cue(getFrameFromCueMessage(transport.cue(), m_Playlist, currentFrame));
            break;
        case Transport_TransportType_CUE_FIRST:
            m_Playback.cue(m_Playlist.getFirstFrame());
            break;
        case Transport_TransportType_CUE_LAST:
            m_Playback.cue(m_Playlist.getLastFrame());
            break;
        case Transport_TransportType_CUE_STORED:
            m_Playback.cue(m_StoredFrame);
            break;
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

        Setup &setup(g_ApplicationRendererSuite.m_Setup);

        // populate images
        setup.m_Images.clear();
        m_FileBufferHolder.update(frame, m_Playlist);
        BOOST_FOREACH( const ImageHolder &image, m_FileBufferHolder.getImages() )
                        setup.m_Images.push_back(image.getImageDescription());

        // populate clips
        m_Playlist.getClipsAtFrame(frame, setup.m_Clips);

        // set current frame
        setup.m_iFrame = frame;

    } catch (exception& e) {
        cerr << HEADER + "Unexpected error while starting simulation step : " << e.what() << endl;
    }
}

OfxRendererSuiteV1::PresentStatus Application::getPresentStatus() {
    const bool present = m_Playback.shouldPresent();
    //        cout << (present ? 'X' : '_');
    return present ? OfxRendererSuiteV1::PRESENT_NEXT_BLANKING : OfxRendererSuiteV1::SKIP_NEXT_BLANKING;
}

void Application::verticalBlanking(bool presented) {
    const auto now(playback::s_Clock.now());
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
        cout << '\r' << round(m_FrameTimings.frequency()) << "FPS";
        return m_bRequestTermination;
    } catch (exception& e) {
        cerr << HEADER + "Unexpected error while finishing simulation step : " << e.what() << endl;
        return true;
    }
}

void Application::pushEvent(unique_ptr<Message>& event) {
    push(m_IO, event);
}

const Message* Application::popEvent(MessageType& type) {
    if (!m_RendererMessages.tryPop(m_RendererMessageHolder))
        return NULL;
    type = m_RendererMessageHolder->messageType();
    return &m_RendererMessageHolder->message();
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
