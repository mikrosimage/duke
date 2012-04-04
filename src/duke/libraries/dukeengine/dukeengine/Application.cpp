#include "Application.h"

#include <player.pb.h>

#include <sequence/parser/details/Utils.h>

#include <dukeapi/sequence/PlaylistHelper.h>

#include <dukeengine/image/ImageToolbox.h>
#include <dukeengine/host/renderer/Renderer.h>

#include <google/protobuf/descriptor.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/locks.hpp>
#include <boost/chrono.hpp>

#include <iostream>
#include <sstream>
#include <cassert>
#include <set>

#include <cstdio>

using namespace ::google::protobuf::serialize;
using namespace ::google::protobuf;
using namespace ::duke::protocol;
using namespace ::std;

static const string HEADER = "[Application] ";

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

Application::Application(const char* rendererFilename, ImageDecoderFactoryImpl &imageDecoderFactory, IMessageIO &io, int &returnCode,
                         const duke::protocol::Cache& cacheConfiguration) :
                m_IO(io), //
                m_ImageDecoderFactory(imageDecoderFactory), //
                m_CacheConfiguration(cacheConfiguration), //
                m_AudioEngine(), //
                m_Cache(m_CacheConfiguration, m_ImageDecoderFactory), //
                m_FileBufferHolder(), //
                m_VbiTimings(VBI, 120), //
                m_FrameTimings(FRAME, 10), //
                m_PreviousFrame(-1), //
                m_StoredFrame(-1), //
                m_bRequestTermination(false), //
                m_bAutoNotifyOnFrameChange(false), //
                m_bForceRefresh(true), //
                m_iReturnCode(returnCode), //
                m_Renderer(buildHost(this), rendererFilename) {
    m_ImageDecoderFactory.dumpDecoderInfos();
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
        cerr << HEADER + "First message must be either Renderer or Quit, ignoring message of type " << descriptor->name() << endl;
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

///////////////////////
// Transport section //
///////////////////////

static inline uint32_t cueClipRelative(const PlaylistHelper &helper, unsigned int currentFrame, int clipOffset) {
    const Ranges & clips = helper.allClips;
    if (clips.empty())
        return currentFrame;
    const Ranges::const_iterator itr = find_if(clips.begin(), clips.end(), boost::bind(&sequence::Range::contains, _1, currentFrame));
    assert(itr!=clips.end());
    const size_t index = distance(clips.begin(), itr);
    const int newIndex = int(index) + clipOffset;
    const int boundIndex = std::max(0, std::min(int(clips.size()) - 1, newIndex));
    return clips[boundIndex].first;
}

static inline uint32_t cueClipAbsolute(const PlaylistHelper &helper, unsigned int currentFrame, unsigned clipIndex) {
    const Ranges & clips = helper.allClips;
    if (clipIndex >= clips.size()) {
        cerr << "Can't cue to clip " << clipIndex << ", there is only " << clips.size() << " clips" << endl;
        return currentFrame;
    }
    return clips[clipIndex].first;
}

static inline uint32_t cueClip(const Transport_Cue& cue, const PlaylistHelper &helper, unsigned int current) {
    return cue.cuerelative() ? cueClipRelative(helper, current, cue.value()) : cueClipAbsolute(helper, current, cue.value());
}

static inline uint32_t cueFrame(const Transport_Cue& cue, const PlaylistHelper &helper, int32_t current) {
    if (cue.cuerelative())
        return helper.range.offsetLoopFrame(current, cue.value()).first;
    else
        return helper.range.clampFrame(cue.value());
}

static inline uint32_t getFrameFromCueMessage(const Transport_Cue& cue, const PlaylistHelper &helper, int32_t current) {
    return cue.cueclip() ? cueClip(cue, helper, current) : cueFrame(cue, helper, current);
}

static inline playback::PlaybackType get(PlaybackState_PlaybackMode mode) {
    switch (mode) {
        case PlaybackState::RENDER:
            return playback::RENDER;
        case PlaybackState::NO_SKIP:
            return playback::REALTIME_NO_SKIP;
        case PlaybackState::DROP_FRAME_TO_KEEP_REALTIME:
            return playback::REALTIME;
        default:
            throw runtime_error("bad enum");
    }
}

void Application::consumePlaybackState(const PlaybackState &playbackState) {
    // update playback
    bool changed = false;
    if (playbackState.has_frameratenumerator()) {
        m_PlaybackState.set_frameratenumerator(playbackState.frameratenumerator());
        if (playbackState.has_frameratedenominator())
            m_PlaybackState.set_frameratedenominator(playbackState.frameratedenominator());
        changed = true;
    }
    if (playbackState.has_loop()) {
        m_PlaybackState.set_loop(playbackState.loop());
        changed = true;
    }
    if (playbackState.has_playbackmode()) {
        m_PlaybackState.set_playbackmode(playbackState.playbackmode());
        changed = true;
    }
    if (changed)
        updatePlaybackState();
}

void Application::updatePlaybackState() {
    using namespace boost::chrono;
    const high_resolution_clock::duration nsPerFrame = playback::nsPerFrame(m_PlaybackState.frameratenumerator(), m_PlaybackState.frameratedenominator());
//    cout << HEADER << "frame time " << duration_cast<milliseconds>(nsPerFrame) << endl;
    m_Playback.init(m_Playlist.range, m_PlaybackState.loop(), nsPerFrame);
    m_Playback.setType(get(m_PlaybackState.playbackmode()));
}

void Application::applyTransport(const Transport& transport) {
    //                        m_AudioEngine.applyTransport(transport);
    const uint32_t currentFrame = m_Playback.frame();
    switch (transport.type()) {
        case Transport::PLAY:
            m_Playback.play(currentFrame, 1);
            m_AudioEngine.sync(m_Playback.playlistTime());
            m_AudioEngine.play();
            break;
        case Transport::STOP:
            m_Playback.stop();
            m_AudioEngine.pause();
            break;
        case Transport::STORE:
            m_StoredFrame = currentFrame;
            break;
        case Transport::CUE:
            m_Playback.cue(getFrameFromCueMessage(transport.cue(), m_Playlist, currentFrame));
            m_AudioEngine.pause();
            break;
        case Transport::CUE_FIRST:
            m_Playback.cue(m_Playlist.range.first);
            m_AudioEngine.pause();
            break;
        case Transport::CUE_LAST:
            m_Playback.cue(m_Playlist.range.last);
            m_AudioEngine.pause();
            break;
        case Transport::CUE_STORED:
            m_Playback.cue(m_StoredFrame);
            m_AudioEngine.pause();
            break;
    }

}

/////////////////////
// Message section //
/////////////////////

void Application::consumeDebug(const Debug &debug) const {
#ifdef __linux__
    cout << "\e[J";
#endif
    for (int i = 0; i < debug.line_size(); ++i) {
        size_t found;
        string line = debug.line(i);
        found = line.find_first_of("%");

        while (found != string::npos) {
            stringstream ss;
            ss << line[found + 1];
            int contentID = atoi(ss.str().c_str());
            if (contentID < debug.content_size())
                line.replace(found, 2, dumpInfo(debug.content(contentID)));
            found = line.find_first_of("%", found + 1);
        }
        cout << line << endl;
    }
#ifdef __linux__
    stringstream ss;
    ss << "\r\e[" << debug.line_size() + 1 << "A";
    cout << ss.str() << endl;
#endif
    if (debug.has_pause())
        ::boost::this_thread::sleep(::boost::posix_time::seconds(debug.pause()));
}

void Application::consumeTransport(const Transport &transport, const MessageHolder_Action action) {
    switch (action) {
        case MessageHolder_Action_CREATE: {
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
            cerr << HEADER + "unknown action for transport message " << MessageHolder_Action_Name(action) << endl;
            break;
        }
    }
}

void Application::consumeScene(const Scene& scene) {
    m_Playlist = PlaylistHelper(scene);
    m_AudioEngine.load(scene);
    updatePlaybackState();
    m_Cache.init(m_Playlist, m_CacheConfiguration);
    m_bForceRefresh = true;
}

struct CacheStateGatherer {
    typedef boost::container::flat_set<unsigned int> IndexSet;
    const PlaylistHelper &playlist;
    vector<IndexSet> frames;
    Info_CacheState &cache;
    CacheStateGatherer(const PlaylistHelper &playlist, Info_CacheState &cache) :
                    playlist(playlist), frames(playlist.tracks.size()), cache(cache) {
    }
    void ingest(const image::WorkUnitIds &ids) {
        for (image::WorkUnitIds::const_iterator itr = ids.begin(); itr != ids.end(); ++itr)
            frames[itr->index.track].insert(itr->index.frame);
    }
    void update() const {
        for (size_t i = 0; i < frames.size(); ++i) {
            Info_CacheState_TrackCache &trackCache = *cache.add_track();
            trackCache.set_name(playlist.scene.track(i).name());
            unsigned step = 0;
            const Ranges ranges = sequence::parser::details::getRangesAndStep(frames[i].begin(), frames[i].end(), step);
            for (Ranges::const_iterator itr = ranges.begin(); itr != ranges.end(); ++itr) {
                FrameRange &range = *trackCache.add_range();
                range.set_first(itr->first);
                range.set_last(itr->last);
            }
        }
    }
};

void Application::updateInfoCacheState(Info_CacheState &infos) const {
    if (!m_Cache.enabled())
        return;
    image::WorkUnitIds ids;
    infos.set_ram(m_Cache.dumpKeys(ids));
    CacheStateGatherer gatherer(m_Playlist, infos);
    gatherer.ingest(ids);
    gatherer.update();
}

void Application::updateInfoPlaybackState(Info_PlaybackState &infos) const {
    infos.set_frame(m_Playback.frame());
    infos.set_fps(m_FrameTimings.frequency());
}

void Application::updateInfoImages(RepeatedPtrField<Info_ImageInfo> &infos) const {
    if (m_FileBufferHolder.getImages().empty())
        return;
    MediaFrames frames;
    m_Playlist.mediaFramesAt(m_Playback.frame(), frames);
    for (MediaFrames::const_iterator itr = frames.begin(); itr != frames.end(); ++itr) {
        const unsigned track = itr->index.track;
        const ImageHolder &holder = m_FileBufferHolder.getImages()[track];
        const ImageDescription &description = holder.getImageDescription();
        Info_ImageInfo &current = *infos.Add();
        current.set_filename(itr->filename());
        current.set_track(track);
        current.set_width(description.width);
        current.set_height(description.height);
        current.set_depth(description.depth);
        current.set_format(toString(description.format));
        current.set_imagesize(description.imageDataSize);
        current.set_filesize(description.fileDataSize);
    }
}

void Application::updateInfoExtensions(RepeatedPtrField<string> &extensions) const {
    const char ** pExtensions = m_ImageDecoderFactory.getAvailableExtensions();
    for (; pExtensions != NULL && *pExtensions != NULL; ++pExtensions)
        *extensions.Add() = *pExtensions;
}

void Application::consumeInfo(Info info, const MessageHolder_Action action) {
    switch (info.content()) {
        case Info_Content_PLAYBACKSTATE:
            updateInfoPlaybackState(*info.mutable_playbackstate());
            break;
        case Info_Content_CACHESTATE:
            updateInfoCacheState(*info.mutable_cachestate());
            break;
        case Info_Content_IMAGEINFO:
            updateInfoImages(*info.mutable_image());
            break;
        case Info_Content_EXTENSIONS:
            updateInfoExtensions(*info.mutable_extension());
            break;
        default:
            return;
    }
    switch (action) {
        case MessageHolder_Action_CREATE:
            info.PrintDebugString();
            break;
        case MessageHolder_Action_RETRIEVE: {
            pushEvent(pack(info));
            break;
        }
        default:
            break;
    }
}

void Application::consumeCache(const Cache &cache, const MessageHolder_Action action) {
    if (action == MessageHolder_Action_CREATE) {
        m_Cache.init(m_Playlist, cache);
    } else if (action == MessageHolder_Action_RETRIEVE) {
        m_Cache.configuration().PrintDebugString();
        pushEvent(pack(m_Cache.configuration()));
    }
}

void Application::consumeMessages() {
    SharedHolder pHolder;
    while (m_IO.tryPop(pHolder)) {
        if (handleQuitMessage(pHolder)) {
            m_bRequestTermination = true;
            cerr << "handling Quit message and quitting" << endl;
            return;
        }
        const MessageHolder &holder = *pHolder;
        const Descriptor* pDescriptor = descriptorFor(holder);
        dump(pDescriptor, holder);
        if (isType<duke::protocol::Renderer>(pDescriptor))
            cerr << HEADER + "calling INIT_RENDERER twice is forbidden" << endl;
        else if (isType<Debug>(pDescriptor))
            consumeDebug(unpackTo<Debug>(holder));
        else if (isType<Scene>(pDescriptor))
            consumeScene(unpackTo<Scene>(holder));
        else if (isType<Transport>(pDescriptor))
            consumeTransport(unpackTo<Transport>(holder), holder.action());
        else if (isType<PlaybackState>(pDescriptor))
            consumePlaybackState(unpackTo<PlaybackState>(holder));
        else if (isType<Info>(pDescriptor))
            consumeInfo(unpackTo<Info>(holder), holder.action());
//        else if (isType<Cache>(pDescriptor))
//            consumeCache(unpackTo<Cache>(holder), holder.action());
        else
            m_RendererMessages.push(pHolder);
    }
}

///////////////////////
// Rendering section //
///////////////////////

void Application::renderStart() {
    try {
        // consume message
        consumeMessages();

        // update current frame
        if (m_Playback.adjustCurrentFrame()) {
#ifdef DEBUG
            cout << "unstable" << endl;
#endif
        }

        const size_t frame = m_Playback.frame();

        // sync audio
        if (m_Playback.playing())
            m_AudioEngine.checksync(m_Playback.playlistTime());

        // retrieve images
        Setup &setup(g_ApplicationRendererSuite.m_Setup);
        if (m_bForceRefresh || m_PreviousFrame != frame) {
            const int32_t speed = m_Playback.getSpeed();
            const EPlaybackState state = speed == 0 ? BALANCE : (speed > 0 ? FORWARD : REVERSE);
            m_Cache.seek(frame, state);
            m_FileBufferHolder.update(frame, m_Cache, m_Playlist);
            m_bForceRefresh = false;
        }

        setup.m_Images.clear();
        BOOST_FOREACH( const ImageHolder &image, m_FileBufferHolder.getImages() )
                {
                    //cout << image.getImageDescription().width << "x" << image.getImageDescription().height << endl;
                    setup.m_Images.push_back(image.getImageDescription());
                }

        // populate clips
        // TODO : need better code... this is awkward
        m_Playlist.clipsAt(frame, setup.m_Clips);

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

///////////////////
// Event section //
///////////////////

void Application::pushEvent(const google::protobuf::serialize::MessageHolder& event) {
    dump(descriptorFor(event), event, true);
    m_IO.push(make_shared(event));
}

const google::protobuf::serialize::MessageHolder * Application::popEvent() {
    m_RendererMessages.tryPop(m_RendererMessageHolder);
    return m_RendererMessageHolder.get();
}

//////////////////
// Dump section //
//////////////////

struct FilenameExtractor {
    const image::WorkUnitId& id;
    FilenameExtractor(const image::WorkUnitId& id) :
                    id(id) {
    }
};

ostream& operator<<(ostream& stream, const FilenameExtractor& fe) {
    return stream << fe.id.filename;
}

string Application::dumpInfo(const Debug_Content& info) const {
    stringstream ss;

    switch (info) {
        case Debug_Content_FRAME:
            ss << m_Playback.frame();
            break;
        case Debug_Content_FILENAMES: {
            image::WorkUnitIds ids;
            ids.reserve(200);
            m_Cache.dumpKeys(ids);
            copy(ids.begin(), ids.end(), ostream_iterator<FilenameExtractor>(ss, "\n"));
//            ss << "found " << ids.size() << " files in cache";
            break;
        }
        case Debug_Content_FPS: {
            ss << m_FrameTimings.frequency();
            break;
        }
    }
    return ss.str();
}

