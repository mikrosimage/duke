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
#include <duke/engine/overlay/StatusOverlay.h>
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
		const std::string exePath = getDirname(getExePath());
		const std::string splashScreenPath = exePath + "/splashscreen";
		if (getFileStatus(splashScreenPath.c_str()) == FileStatus::DIRECTORY) {
			parameters.additionnalOptions.push_back(splashScreenPath);
			timeline = buildTimeline(parameters);
		}
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

static const char* getFitModeString(FitMode &mode) {
	switch (mode) {
	case FitMode::ACTUAL:
		return "Actual pixel";
	case FitMode::INNER:
		return "Fit inner frame";
	case FitMode::FREE:
		return "No fit";
	case FitMode::OUTER:
		return "Fit outer frame";
	}
	throw std::runtime_error("unknown fitmode");
}

bool Duke::keyPressed(int key) const {
	return m_pWindow->glfwGetKey(key) == GLFW_PRESS;
}

bool Duke::hasWindowParam(int param) const {
	return m_pWindow->glfwGetWindowParam(param);
}

bool Duke::togglePlayStop() {
	const int speed = m_Player.getPlaybackSpeed() == 0 ? 1 : 0;
	m_Player.setPlaybackSpeed(speed);
	return speed != 0;
}

void Duke::run() {
	std::map<const IMediaStream*, std::vector<Range> > cacheStateTmp;

	const auto pGlyphRenderer = std::make_shared<GlyphRenderer>();
	AttributesOverlay attributesOverlay(pGlyphRenderer);
	StatusOverlay statusOverlay(pGlyphRenderer);
	bool showAttributesOverlay = false;

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

		// current frame
		const size_t frame = m_Context.currentFrame.round();

		// preparing current frame textures
		auto &textureCache = m_Player.getTextureCache();
		textureCache.ensureReady(frame);

		// rendering tracks
		for (const Track &track : m_Player.getTimeline()) {
			if (track.disabled)
				continue;

			const auto pTrackItr = track.clipContaining(frame);
			if (pTrackItr == track.end())
				continue;

			m_Context.pCurrentImage = nullptr;
			const MediaFrameReference mfr = track.getMediaFrameReferenceAt(frame);
			const auto pMediaStream = mfr.first;
			if (pMediaStream) {
				auto pLoadedTexture = textureCache.getLoadedTexture(mfr);
				if (pLoadedTexture) {
					m_Context.pCurrentImage = pLoadedTexture;
					auto &texture = *pLoadedTexture->pTexture;
					auto boundTexture = texture.scope_bind_texture();
					glTexParameteri(texture.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(texture.target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					renderWithBoundTexture(pSquare.get(), m_Context);
				} else {
					drawText(*pGlyphRenderer, m_Context.viewport, "missing frame", 100, 100, 1, 3);
				}
			}
			const auto& pOverlayTrack = pTrackItr->second.pOverlay;
			if (pOverlayTrack)
				pOverlayTrack->render(m_Context);
			if (showAttributesOverlay)
				attributesOverlay.render(m_Context);
		}
		statusOverlay.render(m_Context);

		// displaying
		m_pWindow->glfwSwapBuffers();

		// updating time
		const auto elapsedMicroSeconds = metronom.tick();
		m_Player.offsetPlaybackTime(elapsedMicroSeconds);
		m_Context.liveTime += Time(elapsedMicroSeconds.count(), 1000000);

		// preparing display function
		const auto display = [&](const std::string &msg) {
			statusOverlay.setString(m_Context.liveTime, msg);
		};
		const auto displayExposure = [&]() {
			std::ostringstream oss;
			oss.precision(3);
			oss << "exposure "<< std::fixed << m_Context.exposure;
			display(oss.str());
		};

		// handling input by char
		auto &charStrokes = m_pWindow->getPendingChars();
		for (const int key : charStrokes) {
			switch (key) {
			case ' ': {
				const bool playing = togglePlayStop();
				display(playing ? "play" : "stop");
				break;
			}
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
				displayExposure();
				break;
			case '-':
				m_Context.exposure /= 1.2;
				displayExposure();
				break;
			case 'o':
				showAttributesOverlay = !showAttributesOverlay;
				break;
			case 'f':
				if (setNextMode(m_Context.fitMode))
					m_pWindow->setPan(glm::ivec2());
				display(getFitModeString(m_Context.fitMode));
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
			case GLFW_KEY_HOME:
				m_Player.cue(m_Player.getTimeline().getRange().first);
				break;
			case GLFW_KEY_END:
				m_Player.cue(m_Player.getTimeline().getRange().last);
				break;
			case GLFW_KEY_LEFT:
				m_Player.cueRelative(ctrlModifier ? -25 : -1);
				break;
			case GLFW_KEY_RIGHT:
				m_Player.cueRelative(ctrlModifier ? 25 : 1);
				break;
			}
		}
		keyStrokes.clear();

		// check stop
		running = !(hasWindowParam(GLFW_SHOULD_CLOSE) || (keyPressed(GLFW_KEY_ESC) && hasWindowParam(GLFW_FOCUSED)));

		// dumping cache state every 50 ms
		const auto now = duke_clock::now();
		if ((now - milestone) > std::chrono::milliseconds(50)) {
			const auto currentWeight = textureCache.getImageCache().dumpState(cacheStateTmp);
			std::cout << (currentWeight / 1024 / 1024) << " MiB";
			for (const auto& pair : cacheStateTmp) {
				std::cout << " " << pair.first;
				for (const auto &range : pair.second)
					std::cout << " [" << range.first << ',' << range.last << ']';
			}
			std::cout << std::endl;

//				metronom.dump();
			milestone = now;
		}
	}
}
} /* namespace duke */
