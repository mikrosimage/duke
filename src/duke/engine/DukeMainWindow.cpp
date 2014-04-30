#include "DukeMainWindow.hpp"

#include <duke/engine/overlay/StatisticsOverlay.hpp>
#include <duke/engine/overlay/OnScreenDisplayOverlay.hpp>
#include <duke/engine/overlay/AttributesOverlay.hpp>
#include <duke/engine/ConsoleIO.hpp>
#include <duke/engine/rendering/ImageRenderer.hpp>
#include <duke/engine/commands/Commands.hpp>
#include <duke/time/Clock.hpp>
#include <duke/gl/GL.hpp>

#include <string>
#include <sstream>


namespace duke {

DukeMainWindow::DukeMainWindow(GLFWwindow *pWindow, const CmdLineParameters &parameters) :
		DukeGLFWWindow(pWindow), m_CmdLine(parameters), m_Player(parameters), m_GlyphRenderer(m_GeometryRenderer) {
	m_Context.pGlyphRenderer = &m_GlyphRenderer;
	m_Context.pGeometryRenderer = &m_GeometryRenderer;
	m_Context.fileColorSpace = parameters.inputColorSpace;
	m_Context.screenColorSpace = parameters.outputColorSpace;

	::glfwMakeContextCurrent(m_pWindow);
	::glfwGetWindowSize(m_pWindow, &m_WindowDim.x, &m_WindowDim.y);
	::glfwGetWindowPos(m_pWindow, &m_WindowPos.x, &m_WindowPos.y);
	::glfwSwapInterval(parameters.swapBufferInterval);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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

	// registering commands
	using namespace cmd;
	m_Commands.addAndBind<ArgsCmd>( { "args", "display the command's arguments", { cmd::cmd } }, std::cref(m_Commands));
	m_Commands.addAndBind<ManCmd>( { "man", "display the command's help", { cmd::cmd } }, std::cref(m_Commands));
	m_Commands.addAndBind<SuggestCmd>( { "cmds", "suggest a list of commands", { value } }, std::cref(m_Commands));
	m_Commands.addAndBind<SuggestParam>( { "params", "suggest a list of parameters", { value } }, std::cref(m_Parameters));
	m_Commands.addAndBind<FunctionCmd>( { "play", "play the current playlist" }, [&]() {m_Player.setPlaybackSpeed(1);});
	m_Commands.addAndBind<FunctionCmd>( { "stop", "stop playback" }, [&]() {m_Player.setPlaybackSpeed(0);});
	m_Commands.addAndBind<FunctionCmd>( { "begin", "move to the first frame" }, [&]() {m_Player.cue(m_Player.getTimeline().getRange().first);});
	m_Commands.addAndBind<FunctionCmd>( { "end", "move to the last frame" }, [&]() {m_Player.cue(m_Player.getTimeline().getRange().last);});
	m_Commands.addAndBind<FunctionCmd>( { "quit", "quit the application" }, [&]() {glfwSetWindowShouldClose(getHandle(),true);});
//	m_Commands.addAndBind<LsCmd>( { "ls", "list the current directory", { path } });
//	m_Commands.addAndBind<NoOpCmd>( { "next", "move to next clip" });
//	m_Commands.addAndBind<NoOpCmd>( { "previous", "move to previous clip" });
	std::cout << "Type 'cmds' to list available commands" << std::endl;
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
	m_Context.resetFitMode = true;
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
	const auto oldZoom = m_Context.zoom;
	auto newZoom = oldZoom;
	newZoom = logf(newZoom);
	newZoom += y / 8;
	newZoom = expf(newZoom);
	m_Context.zoom = newZoom;

	// computing mouse position in image space
	glm::vec2 mousePos = glm::vec2(m_MousePos.x, m_Context.viewport.dimension.y - m_MousePos.y - 1);
	mousePos -= m_Context.viewport.dimension / 2;
	mousePos -= m_Context.pan;
	m_Context.pan -= glm::ivec2(mousePos * ((newZoom / oldZoom) - 1));
}

void DukeMainWindow::onMouseDrag(int dx, int dy) {
	m_Context.pan.x += dx;
	m_Context.pan.y -= dy;
}

namespace { // defining channel mask constants

using glm::bvec4;
const auto r = bvec4(true, false, false, false);
const auto g = bvec4(false, true, false, false);
const auto b = bvec4(false, false, true, false);
const auto a = bvec4(false, false, false, true);
const auto all = bvec4(false);

bool setNextMode(FitMode &mode) {
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

const char* getFitModeString(FitMode &mode) {
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

}  // namespace

void DukeMainWindow::run() {
	ConsoleIO console;
	std::vector<std::string> commands;
	AttributesOverlay metadataOverlay(m_GlyphRenderer);
	OnScreenDisplayOverlay statusOverlay(m_GlyphRenderer);
	StatisticsOverlay statisticOverlay(m_GlyphRenderer, m_Player.getTimeline());
	bool showMetadataOverlay = false;
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

	const auto setupZoom = [&]() {
		if(m_Context.fitMode == FitMode::FREE||!m_Context.resetFitMode)
		return;
		m_Context.zoom = getZoomValue(m_Context);
		m_Context.pan= glm::ivec2();
		m_Context.resetFitMode = false;
	};

	while (running) {
		// fetching user inputs
		::glfwPollEvents();

		// setting up context
		m_Context.viewport = Viewport(glm::ivec2(), m_WindowDim);
		m_Context.currentFrame = m_Player.getCurrentFrame();
		m_Context.playbackTime = m_Player.getPlaybackTime();

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
			m_Context.pCurrentMediaStream = nullptr;
			const MediaFrameReference mfr = track.getMediaFrameReferenceAt(frame);
			const auto pMediaStream = mfr.pStream;


			if (pMediaStream) {
				auto pLoadedTexture = textureCache.getLoadedTexture(mfr);
				// Mode where we force the texture to be ready before displaying something
				if (m_CmdLine.unlimitedFPS) {
					int MaxTry = 1000;
					while (!pLoadedTexture) {
						// The texture was not ready, re-ask for it
						textureCache.prepare(frame, mode);
						pLoadedTexture = textureCache.getLoadedTexture(mfr);
						if (MaxTry--<=0)
							continue; 
					}
				}
				if (pLoadedTexture) {
					m_Context.pCurrentImage = pLoadedTexture;
					m_Context.pCurrentMediaStream = pMediaStream;
					setupZoom();
					auto &texture = *pLoadedTexture->pTexture;
					auto boundTexture = texture.scope_bind_texture();
					glTexParameteri(texture.target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(texture.target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					renderWithBoundTexture(m_GlyphRenderer.getGeometryRenderer().shaderPool, pSquare.get(), m_Context);
				} else {
					drawText(m_GlyphRenderer, m_Context.viewport, "caching", 100, 100, 1, 3);
				}
			}
			const auto& pOverlayTrack = pTrackItr->second.pOverlay;
			if (pOverlayTrack)
				pOverlayTrack->render(m_Context);
			if (showMetadataOverlay)
				metadataOverlay.render(m_Context);
		}
		if (showStatisticOverlay)
			statisticOverlay.render(m_Context);
		statusOverlay.render(m_Context);

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
			case 'm':
				showMetadataOverlay = !showMetadataOverlay;
				break;
			case 's':
				showStatisticOverlay = !showStatisticOverlay;
				break;
			case 'f':
				setNextMode(m_Context.fitMode);
				m_Context.resetFitMode = true;
				display(getFitModeString(m_Context.fitMode));
				break;
			}
		}
		m_CharStrokes.clear();

		// handling input by key
		const bool ctrlModifier = keyPressed(GLFW_KEY_LEFT_CONTROL) || keyPressed(GLFW_KEY_RIGHT_CONTROL);
        const bool shiftModifier = keyPressed(GLFW_KEY_LEFT_SHIFT) || keyPressed(GLFW_KEY_RIGHT_SHIFT);
        const int seekAmount = shiftModifier && ctrlModifier ? 250 : (ctrlModifier ? 25 : 1);
		for (const int key : m_KeyStrokes) {
			switch (key) {
			case GLFW_KEY_HOME:
				commands.emplace_back("begin");
				break;
			case GLFW_KEY_END:
				commands.emplace_back("end");
				break;
			case GLFW_KEY_LEFT:
				m_Player.cueRelative(-seekAmount);
				break;
			case GLFW_KEY_RIGHT:
				m_Player.cueRelative(seekAmount);
				break;
			}
		}
		m_KeyStrokes.clear();

		// checking incoming commands
		console.poll(commands);
		for (const auto& cmd : commands) {
			auto result = m_Commands.execute(cmd);
			if (!result.empty())
				std::cout << result << std::endl;
		}
		commands.clear();

		// check stop
		running = !(shouldClose() || (keyPressed(GLFW_KEY_ESCAPE)));

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
