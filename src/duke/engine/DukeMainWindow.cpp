/*
 * DukeMainWindow.cpp
 *
 *  Created on: Mar 3, 2013
 *      Author: Guillaume Chatelet
 */

#include "DukeMainWindow.h"

#include <duke/engine/overlay/StatisticsOverlay.h>
#include <duke/engine/overlay/OnScreenDisplayOverlay.h>
#include <duke/engine/overlay/AttributesOverlay.h>
#include <duke/engine/rendering/ImageRenderer.h>
#include <duke/time/Clock.h>
#include <duke/gl/GL.h>

#include <string>
#include <sstream>

namespace duke {

DukeMainWindow::DukeMainWindow(GLFWwindow *pWindow, const CmdLineParameters &parameters) :
		DukeGLFWWindow(pWindow), m_CmdLine(parameters), m_Player(parameters), m_GlyphRenderer(m_GeometryRenderer), m_Pan(m_Context.pan), m_Zoom(m_Context.zoom) {
	m_Context.pGlyphRenderer = &m_GlyphRenderer;
	m_Context.pGeometryRenderer = &m_GeometryRenderer;

	::glfwMakeContextCurrent(m_pWindow);
	::glfwGetWindowSize(m_pWindow, &m_WindowDim.x, &m_WindowDim.y);
	::glfwGetWindowPos(m_pWindow, &m_WindowPos.x, &m_WindowPos.y);
	::glfwSwapInterval(parameters.swapBufferInterval);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	using std::bind;
	using std::placeholders::_1;
	using std::placeholders::_2;

	mouseButtonCallback = bind(&DukeMainWindow::onMouseClick, this, _1, _2);
	mousePosCallback = bind(&DukeMainWindow::onMouseMove, this, _1, _2);
	scrollCallback = bind(&DukeMainWindow::onScroll, this, _1, _2);
	windowResizeCallback = bind(&DukeMainWindow::onWindowResize, this, _1, _2);
	charCallback = bind(&DukeMainWindow::onChar, this, _1);
	keyCallback = bind(&DukeMainWindow::onKey, this, _1, _2);
	registerCallbacks();

}

void DukeMainWindow::load(const Timeline& timeline, const FrameDuration& frameDuration, const FitMode fitMode, int speed) {
	m_Player.load(timeline, frameDuration);
	m_Player.setPlaybackSpeed(speed);
	m_Context.fitMode = fitMode;
}

void DukeMainWindow::onKey(int key, int action) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
		m_KeyStrokes.push_back(key);
}

void DukeMainWindow::onChar(unsigned int unicodeCodePoint) {
	m_CharStrokes.push_back(unicodeCodePoint);
}

void DukeMainWindow::onWindowResize(int width, int height) {
	::glViewport(0, 0, width, height);
	m_WindowDim.x = width;
	m_WindowDim.y = height;
}

void DukeMainWindow::onMouseMove(int x, int y) {
	int dx = x - m_MousePos.x;
	int dy = y - m_MousePos.y;
	m_MousePos.x = x;
	m_MousePos.y = y;
	if (m_MouseLeftDown)
		onMouseDrag(dx, dy);
}

void DukeMainWindow::onMouseClick(int buttonId, int buttonState) {
	if (buttonId == GLFW_MOUSE_BUTTON_LEFT)
		m_MouseLeftDown = buttonState == GLFW_PRESS;
}

void DukeMainWindow::onScroll(double x, double y) {
	m_Zoom += y;
}

void DukeMainWindow::onMouseDrag(int dx, int dy) {
	m_Pan.x += dx;
	m_Pan.y -= dy;
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
	case FitMode::FREE:
		mode = FitMode::INNER;
		return true;
	case FitMode::INNER:
		mode = FitMode::OUTER;
		return true;
	case FitMode::OUTER:
		mode = FitMode::ACTUAL;
		return false;
	case FitMode::ACTUAL:
		mode = FitMode::INNER;
		return true;
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

void DukeMainWindow::run() {
	AttributesOverlay attributesOverlay(m_GlyphRenderer);
	OnScreenDisplayOverlay statusOverlay(m_GlyphRenderer);
	StatisticsOverlay statisticOverlay(m_GlyphRenderer, m_Player.getTimeline());
	bool showAttributesOverlay = false;
	bool showStatisticOverlay = true;

	SharedMesh pSquare = createSquare();

	size_t lastFrame = 0;
	auto milestone = duke_clock::now();
	bool running = true;

	const auto keyPressed = [=](int key) -> bool {
		return ::glfwGetKey(m_pWindow, key) == GLFW_PRESS;
	};

	const auto shouldClose = [=]() -> bool {
		return ::glfwWindowShouldClose(m_pWindow);
	};

	const auto togglePlayStop = [&]() -> bool {
		const int speed = m_Player.getPlaybackSpeed() == 0 ? 1 : 0;
		m_Player.setPlaybackSpeed(speed);
		return speed != 0;
	};

	const auto display = [&](const std::string &msg) {
		statusOverlay.setString(m_Context.liveTime, msg);
	};

	const auto displayExposure = [&]() {
		std::ostringstream oss;
		oss.precision(3);
		oss << "exposure "<< std::fixed << m_Context.exposure;
		display(oss.str());
	};

	while (running) {
		// fetching user inputs
		::glfwPollEvents();

		// checking pan position or zoom changed
		const bool notFreeMode = m_Context.fitMode != FitMode::FREE;
		const bool panChanged = m_Context.pan != m_Pan;
		const bool zoomChanged = m_Context.zoom != m_Zoom;
		const bool switchToFreeFitMode = notFreeMode && (panChanged || zoomChanged);
		if (switchToFreeFitMode) {
			m_Context.fitMode = FitMode::FREE;
			m_Pan = glm::ivec2();
			m_Zoom = 0;
		}

		// setting up context
		m_Context.viewport = Viewport(glm::ivec2(), m_WindowDim);
		m_Context.currentFrame = m_Player.getCurrentFrame();
		m_Context.playbackTime = m_Player.getPlaybackTime();
		m_Context.pan = m_Pan;
		m_Context.zoom = m_Zoom;

		// current frame
		const size_t frame = m_Context.currentFrame.round();

		// preparing current frame textures
		auto &textureCache = m_Player.getTextureCache();
		const auto speed = m_Player.getPlaybackSpeed();
		const auto mode = speed < 0 ? IterationMode::BACKWARD : (speed > 0 ? IterationMode::FORWARD : IterationMode::PINGPONG);
		textureCache.prepare(frame, mode);

		// rendering tracks
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
					renderWithBoundTexture(m_GlyphRenderer.getGeometryRenderer().shaderPool, pSquare.get(), m_Context);
				} else {
					drawText(m_GlyphRenderer, m_Context.viewport, "missing frame", 100, 100, 1, 3);
				}
			}
			const auto& pOverlayTrack = pTrackItr->second.pOverlay;
			if (pOverlayTrack)
				pOverlayTrack->render(m_Context);
			if (showAttributesOverlay)
				attributesOverlay.render(m_Context);
		}
		statusOverlay.render(m_Context);
		if (showStatisticOverlay)
			statisticOverlay.render(m_Context);

		// displaying
		::glfwSwapBuffers(m_pWindow);

		// updating time
		const auto elapsedMicroSeconds = statisticOverlay.vBlankMetronom.tick();
		const Time offset = m_CmdLine.unlimitedFPS ? m_Player.getFrameDuration() : Time(elapsedMicroSeconds);
		m_Player.offsetPlaybackTime(offset);
		m_Context.liveTime += Time(elapsedMicroSeconds.count(), 1000000);

		if (frame != lastFrame) {
			statisticOverlay.frameMetronom.tick();
			lastFrame = frame;
		}

		// handling input by char
		for (const int key : m_CharStrokes) {
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
			case '*':
				m_Context.exposure = 1;
				displayExposure();
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
			case 's':
				showStatisticOverlay = !showStatisticOverlay;
				break;
			case 'f':
				setNextMode(m_Context.fitMode);
				m_Context.pan = m_Pan = glm::ivec2();
				m_Context.zoom = m_Zoom = 0;
				display(getFitModeString(m_Context.fitMode));
				break;
			}
		}
		m_CharStrokes.clear();

		// handling input by key
		const bool ctrlModifier = keyPressed(GLFW_KEY_LEFT_CONTROL) || keyPressed(GLFW_KEY_RIGHT_CONTROL);
		//		const bool shiftModifier = keyPressed(GLFW_KEY_LEFT_SHIFT) || keyPressed(GLFW_KEY_RIGHT_SHIFT);
		for (const int key : m_KeyStrokes) {
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
		m_KeyStrokes.clear();

		// check stop
		running = !(shouldClose() || (keyPressed(GLFW_KEY_ESC)));

		// dumping cache state every 200 ms
		const auto now = duke_clock::now();
		if ((now - milestone) > std::chrono::milliseconds(100)) {
			textureCache.getImageCache().dumpState(statisticOverlay.cacheState);
			statisticOverlay.vBlankMetronom.compute();
			statisticOverlay.frameMetronom.compute();
			milestone = now;
		}
	}
}

} /* namespace duke */
