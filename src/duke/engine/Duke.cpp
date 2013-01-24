/*
 * Duke.cpp
 *
 *  Created on: Jan 6, 2013
 *      Author: Guillaume Chatelet
 */

#include "Duke.h"
#include <duke/cmdline/CmdLineParameters.h>
#include <duke/filesystem/FsUtils.h>
#include <duke/time/Clock.h>
#include <duke/engine/Context.h>
#include <duke/engine/rendering/ImageRenderer.h>
#include <duke/engine/rendering/GlyphRenderer.h>
#include <duke/engine/streams/SingleFrameStream.h>
#include <duke/engine/streams/FileInfoOverlay.h>
#include <duke/gl/GL.h>
#include <glm/glm.hpp>
#include <sequence/Parser.hpp>

#include <sstream>

namespace duke {

const char * const pMetadataTrack = "metadata";

static sequence::Configuration getParserConf() {
    using namespace sequence;
    Configuration conf;
    conf.sort = true;
    conf.bakeSingleton = true;
    conf.mergePadding = true;
    conf.pack = true;
    return conf;
}

static glm::ivec2 getDesktopDimensions() {
    GLFWvidmode desktopResolution;
    glfwGetDesktopMode(&desktopResolution);
    return glm::ivec2(desktopResolution.Width, desktopResolution.Height);
}

Timeline buildTimeline(const CmdLineParameters &parameters) {
    const auto& paths = parameters.additionnalOptions;
    if (paths.empty())
        throw commandline_error("nothing to do, specify at least one file or directory");
    Track track;
    size_t offset = 0;
    for (const std::string &path : paths) {
        const auto fileStatus = getFileStatus(path.c_str());
        switch (fileStatus) {
            case FileStatus::NOT_A_FILE:
                throw commandline_error("'" + path + "' is not a file nor a directory");
            case FileStatus::FILE:
                track.add(offset, Clip { 1, std::make_shared<SingleFrameStream>(path.c_str()) });
                ++offset;
                break;
            case FileStatus::DIRECTORY: {
                using namespace sequence;
                for (const Item &item : parseDir(getParserConf(), path.c_str()).files)
                    switch (item.getType()) {
                        case Item::SINGLE:
                            track.add(offset, Clip { 1, std::make_shared<SingleFrameStream>((path + '/' + item.filename).c_str()) });
                            ++offset;
                            break;
                        case Item::INVALID:
                            throw commandline_error("invalid item");
                        case Item::INDICED:
                        case Item::PACKED:
                            break;
                    }
                break;
            }
        }
    }
    if (track.empty())
        throw commandline_error("nothing to play");
    // find textures here : http://dwarffortresswiki.org/index.php/Tileset_repository
    const auto pGlyphRenderer = std::make_shared<GlyphRenderer>(".duke_ascii_font");
    Track overlay;
    overlay.disabled = true;
    overlay.name = pMetadataTrack;
    for (const auto& pair : track) {
        const auto& pStream = pair.second.pStream;
        overlay.add(pair.first, Clip { pair.second.frames, std::make_shared<FileInfoOverlay>(pGlyphRenderer, std::static_pointer_cast<SingleFrameStream>(pStream)) });
    }
    return {track, overlay};
}

Duke::Duke(const CmdLineParameters &parameters) {
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// setup window
    const auto windowMode = parameters.fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW;
    auto desktopDimensions = getDesktopDimensions();
    if (!parameters.fullscreen)
        desktopDimensions /= 2;
    m_Window.openWindow(desktopDimensions.x, desktopDimensions.y, 0, 0, 0, 0, 0, 0, windowMode);

// GL setup
    glfwSwapInterval(parameters.swapBufferInterval);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

// initializing timeline
    m_Player.load(buildTimeline(parameters), FrameDuration::PAL);
}

namespace { // defining channel mask constants

using glm::bvec4;
static const auto r = bvec4(true, false, false, false);
static const auto g = bvec4(false, true, false, false);
static const auto b = bvec4(false, false, true, false);
static const auto a = bvec4(false, false, false, true);
static const auto all = bvec4(false);

}  // namespace

void Duke::run() {
    Context context;
    SharedMesh pSquare = getSquare();
    context.renderTexture = [&](const ITexture &texture, const Attributes& attributes) {
        render(pSquare.get(), texture, attributes, context);
    };
    Metronom metronom(100);
    auto milestone = duke_clock::now();
    bool running = true;
    while (running) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // setting up context
        context.viewport = m_Window.useViewport(false, false, false, false);
        context.currentFrame = m_Player.getCurrentFrame();
        context.playbackTime = m_Player.getPlaybackTime();
        context.pan = m_Window.getRelativeMousePos();
        context.zoom = glfwGetMouseWheel();

        // rendering
        for (const Track &track : m_Player.getTimeline()) {
            if (track.disabled)
                continue;
            const MediaFrameReference mfr = track.getClipFrame(context.currentFrame.round());
            const Clip* pClip = mfr.first;
            if (!pClip)
                continue;
            const auto& pStream = pClip->pStream;
            if (!pStream)
                continue;
            context.clipFrame = mfr.second;
            pStream->doRender(context);
        }

        // displaying
        glfwSwapBuffers();

        // updating time
        const auto elapsedMicroSeconds = metronom.tick();
        m_Player.offsetPlaybackTime(elapsedMicroSeconds);

        // handling input
        auto &keyStrokes = m_Window.getPendingKeys();
        for (const int key : keyStrokes) {
            switch (key) {
                case ' ':
                    m_Player.setPlaybackSpeed(m_Player.getPlaybackSpeed() == 0 ? 1 : 0);
                    break;
                case '4':
                    m_Player.setPlaybackSpeed(-1);
                    m_Player.offsetPlaybackTime(m_Player.getFrameDuration());
                    m_Player.setPlaybackSpeed(0);
                    break;
                case '6':
                    m_Player.setPlaybackSpeed(1);
                    m_Player.offsetPlaybackTime(m_Player.getFrameDuration());
                    m_Player.setPlaybackSpeed(0);
                    break;
                case 'r':
                    context.channels = context.channels == r ? all : r;
                    break;
                case 'g':
                    context.channels = context.channels == g ? all : g;
                    break;
                case 'b':
                    context.channels = context.channels == b ? all : b;
                    break;
                case 'a':
                    context.channels = context.channels == a ? all : a;
                    break;
                case '+':
                    context.exposure *= 1.2;
                    break;
                case '-':
                    context.exposure /= 1.2;
                    break;
                case 'o': {
                    auto pTrack = m_Player.m_Timeline.findTrack(pMetadataTrack);
                    if (pTrack) {
                        bool &disabled = pTrack->disabled;
                        disabled = !disabled;
                    }
                    break;
                }
            }
        }
        keyStrokes.clear();

        // check stop
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

        // dump info every seconds
        const auto now = duke_clock::now();
        if ((now - milestone) > std::chrono::seconds(1)) {
//				metronom.dump();
            milestone = now;
        }
    }
}
} /* namespace duke */
