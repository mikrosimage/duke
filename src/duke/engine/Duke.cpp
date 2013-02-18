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
#include <duke/engine/rendering/ImageRenderer.h>
#include <duke/engine/rendering/GlyphRenderer.h>
#include <duke/engine/overlay/DukeSplashStream.h>
#include <duke/engine/overlay/AttributesOverlay.h>
#include <duke/engine/streams/DiskMediaStream.h>
#include <duke/attributes/AttributeKeys.h>
#include <duke/gl/GL.h>
#include <glm/glm.hpp>
#include <sequence/Parser.hpp>

#include <sstream>

using sequence::Item;

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

Timeline buildTimeline(const CmdLineParameters &parameters) {
	const auto& paths = parameters.additionnalOptions;
	Track track;
	size_t offset = 0;
	for (const std::string &path : paths) {
		const std::string absolutePath = getAbsoluteFilename(path.c_str());
		switch (getFileStatus(absolutePath.c_str())) {
		case FileStatus::NOT_A_FILE:
			throw commandline_error("'" + absolutePath + "' is not a file nor a directory");
		case FileStatus::FILE:
			track.add(offset, Clip { 1, std::make_shared<DiskMediaStream>(Item(absolutePath)), nullptr });
			++offset;
			break;
		case FileStatus::DIRECTORY:
			for (Item item : sequence::parseDir(getParserConf(), absolutePath.c_str()).files) {
				const auto type = item.getType();
				if (type == Item::INVALID)
					throw commandline_error("invalid item while parsing directory");
				if (!item.filename.empty() && item.filename[0] == '.')
					continue; // escaping hidden file
				item.filename = absolutePath + '/' + item.filename;
				switch (type) {
				case Item::SINGLE:
					track.add(offset, Clip { 1, std::make_shared<DiskMediaStream>(item), nullptr });
					++offset;
					break;
				case Item::PACKED: {
					const auto count = item.end - item.start + 1;
					track.add(offset, Clip { count, std::make_shared<DiskMediaStream>(item), nullptr });
					offset += count;
				}
				case Item::INDICED:
				default:
					break;
				}
			}
			break;
		}
	}
	return {track};
}

Duke::Duke(CmdLineParameters parameters) :
		m_Player(parameters) {
	const bool fullscreen = parameters.fullscreen;
	GLFWmonitor* pPrimaryMonitor = glfwGetPrimaryMonitor();
	if (pPrimaryMonitor == nullptr) {
		int monitors = 0;
		GLFWmonitor **pMonitors = nullptr;
		pMonitors = glfwGetMonitors(&monitors);
		if (monitors == 0 || pMonitors == nullptr)
			throw std::runtime_error("No monitor detected");
		pPrimaryMonitor = pMonitors[0];
	}
	GLFWvidmode desktopDefinition = glfwGetVideoMode(pPrimaryMonitor);
	auto windowDefinition = glm::ivec2(desktopDefinition.width, desktopDefinition.height);
	if (!fullscreen)
		windowDefinition /= 2;
	m_pWindow.reset(m_Application.createWindow<DukeWindow>(windowDefinition.x, windowDefinition.y, "", fullscreen ? pPrimaryMonitor : nullptr, nullptr));
	m_pWindow->makeContextCurrent();
	m_pWindow->onWindowResize(windowDefinition.x, windowDefinition.y);

	glfwSwapInterval(parameters.swapBufferInterval);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	auto timeline = buildTimeline(parameters);
	const bool demoMode = timeline.empty();
	if (demoMode) {
		parameters.additionnalOptions.emplace_back("splashscreen");
		timeline = buildTimeline(parameters);
		const Range range = timeline.empty() ? Range(0, 0) : timeline.getRange();
		Track overlay;
		overlay.add(range.first, Clip { range.count(), nullptr, std::make_shared<DukeSplashStream>() });
		timeline.push_back(overlay);
		m_Context.fitMode = FitMode::OUTER;
		m_Player.load(timeline, FrameDuration(1, 15));
		m_Player.setPlaybackSpeed(1);
	} else
		m_Player.load(timeline, FrameDuration::PAL);
}

namespace { // defining channel mask constants

using glm::bvec4;
static const auto r = bvec4(true, false, false, false);
static const auto g = bvec4(false, true, false, false);
static const auto b = bvec4(false, false, true, false);
static const auto a = bvec4(false, false, false, true);
static const auto all = bvec4(false);

}  // namespace

static bool setNextMode(FitMode &mode) {
	switch (mode) {
	case FitMode::ACTUAL:
		mode = FitMode::INNER;
		return true;
	case FitMode::INNER:
		mode = FitMode::OUTER;
		return true;
	case FitMode::FREE:
		mode = FitMode::ACTUAL;
		return true;
	case FitMode::OUTER:
		mode = FitMode::FREE;
		return false;
	}
	throw std::runtime_error("unknown fitmode");
}

bool Duke::keyPressed(int key) const {
	return m_pWindow->glfwGetKey(key) == GLFW_PRESS;
}

bool Duke::hasWindowParam(int param) const {
	return m_pWindow->glfwGetWindowParam(param);
}

void Duke::cue(int offset) {
	m_Player.setPlaybackSpeed(offset);
	m_Player.offsetPlaybackTime(m_Player.getFrameDuration());
	m_Player.setPlaybackSpeed(0);
}

void Duke::togglePlayStop() {
	m_Player.setPlaybackSpeed(m_Player.getPlaybackSpeed() == 0 ? 1 : 0);
}

void Duke::run() {
// find textures here : http://dwarffortresswiki.org/index.php/Tileset_repository
	const auto pGlyphRenderer = std::make_shared<GlyphRenderer>();
	AttributesOverlay overlay(pGlyphRenderer);
	bool showOverlay = false;

	SharedMesh pSquare = getSquare();
	Metronom metronom(100);
	auto milestone = duke_clock::now();
	bool running = true;
	while (running) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// setting up context
		m_Context.viewport = m_pWindow->useViewport(false, false, false, false);
		m_Context.currentFrame = m_Player.getCurrentFrame();
		m_Context.playbackTime = m_Player.getPlaybackTime();
		m_Context.pan = m_pWindow->getPanPos();
		m_Context.zoom = m_pWindow->getScrollPos().y;

		//current frame
		const size_t frame = m_Context.currentFrame.round();

		auto &textureCache = m_Player.getTextureCache();
		textureCache.ensureReady(frame);

		// rendering
		for (const Track &track : m_Player.getTimeline()) {
			if (track.disabled)
				continue;
			const auto pTrackItr = track.clipContaining(frame);
			if (pTrackItr == track.end())
				continue;
			m_Context.pCurrentImage = nullptr;
			const MediaFrameReference mfr = track.getMediaFrameReferenceAt(m_Context.currentFrame.round());
			auto pLoadedTexture = textureCache.getLoadedTexture(mfr);
			if (pLoadedTexture) {
				m_Context.pCurrentImage = pLoadedTexture;
				auto boundTexture = pLoadedTexture->pTexture->scope_bind_texture();
				glTexParameteri(pLoadedTexture->pTexture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(pLoadedTexture->pTexture->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				renderWithBoundTexture(pSquare.get(), m_Context);
			}
			const auto& pOverlayTrack = pTrackItr->second.pOverlay;
			if (pOverlayTrack)
				pOverlayTrack->render(m_Context);
			if (showOverlay)
				overlay.render(m_Context);
		}

		// displaying
		m_pWindow->glfwSwapBuffers();

		// updating time
		const auto elapsedMicroSeconds = metronom.tick();
		m_Player.offsetPlaybackTime(elapsedMicroSeconds);
		m_Context.liveTime += Time(elapsedMicroSeconds.count(), 1000000);

		// handling input by char
		auto &charStrokes = m_pWindow->getPendingChars();
		for (const int key : charStrokes) {
			switch (key) {
			case ' ':
				togglePlayStop();
				break;
			case '4':
				cue(-1);
				break;
			case '6':
				cue(1);
				break;
			case 'p':
				m_pWindow->setPan(glm::ivec2());
				break;
			case 'z':
				m_pWindow->setScroll(glm::vec2());
				break;
			case 'r':
				m_Context.channels = m_Context.channels == r ? all : r;
				break;
			case 'g':
				m_Context.channels = m_Context.channels == g ? all : g;
				break;
			case 'b':
				m_Context.channels = m_Context.channels == b ? all : b;
				break;
			case 'a':
				m_Context.channels = m_Context.channels == a ? all : a;
				break;
			case '+':
				m_Context.exposure *= 1.2;
				break;
			case '-':
				m_Context.exposure /= 1.2;
				break;
			case 'o':
				showOverlay = !showOverlay;
				break;
			case 'f':
				if (setNextMode(m_Context.fitMode))
					m_pWindow->setPan(glm::ivec2());
				break;
			}
		}
		charStrokes.clear();

		// handling input by key
		auto &keyStrokes = m_pWindow->getPendingKeys();
		const bool ctrlModifier = keyPressed(GLFW_KEY_LEFT_CONTROL) || keyPressed(GLFW_KEY_RIGHT_CONTROL);
//		const bool shiftModifier = keyPressed(GLFW_KEY_LEFT_SHIFT) || keyPressed(GLFW_KEY_RIGHT_SHIFT);
		for (const int key : keyStrokes) {
			switch (key) {
			case GLFW_KEY_LEFT:
				cue(ctrlModifier ? -25 : -1);
				break;
			case GLFW_KEY_RIGHT:
				cue(ctrlModifier ? 25 : 1);
				break;
			}
		}
		keyStrokes.clear();

		// check stop
		running = !(hasWindowParam(GLFW_SHOULD_CLOSE) || (keyPressed(GLFW_KEY_ESC) && hasWindowParam(GLFW_FOCUSED)));

		// dump info every seconds
		const auto now = duke_clock::now();
		if ((now - milestone) > std::chrono::seconds(1)) {
//				metronom.dump();
			milestone = now;
		}
	}
}
} /* namespace duke */
