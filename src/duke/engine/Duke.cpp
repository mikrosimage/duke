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
#include <duke/engine/overlay/DukeSplashStream.h>
#include <duke/engine/streams/DiskMediaStream.h>
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
					throw commandline_error("invalid item");
				item.filename = absolutePath + '/' + item.filename;
				switch (type) {
				case Item::SINGLE:
					track.add(offset, Clip { 1, std::make_shared<DiskMediaStream>(item), nullptr });
					++offset;
					break;
				case Item::INDICED:
				case Item::PACKED:
				default:
					break;
				}
			}
			break;
		}
	}
	if (track.empty())
		track.add(offset, Clip { 1, nullptr, std::make_shared<DukeSplashStream>() });
	// find textures here : http://dwarffortresswiki.org/index.php/Tileset_repository
//	const auto pGlyphRenderer = std::make_shared<GlyphRenderer>();
//	Track overlay;
//	overlay.disabled = true;
//	overlay.name = pMetadataTrack;
//	for (const auto& pair : track) {
//		const auto& pStream = pair.second.pStream;
//		overlay.add(pair.first, Clip { pair.second.frames, nullptr, std::make_shared<FileInfoOverlay>(pGlyphRenderer, std::static_pointer_cast<SingleFrameStream>(pStream)) });
//	}
	return {track/*, overlay*/};
}

Duke::Duke(const CmdLineParameters &parameters) {
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
	PackedFrame packedFrame;
//	LoadedTextureCache loadedTextureCache;
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
		context.viewport = m_pWindow->useViewport(false, false, false, false);
		context.currentFrame = m_Player.getCurrentFrame();
		context.playbackTime = m_Player.getPlaybackTime();
		context.pan = m_pWindow->getRelativeMousePos();
		context.zoom = 0;  //glfwGetScrollOffset();

		const size_t frame = context.currentFrame.round();
		// rendering
		for (const Track &track : m_Player.getTimeline()) {
			if (track.disabled)
				continue;
			const auto pTrackItr = track.clipContaining(frame);
			if (pTrackItr == track.end())
				continue;
			const MediaFrameReference mfr = track.getMediaFrameReferenceAt(context.currentFrame.round());
			if (mfr.first) {
//				context.clipFrame = mfr.second;
//				std::shared_ptr<TextureRectangle> pTexture = loadedTextureCache.get(mfr);
//				if (!pTexture) {
//					const bool loaded = m_Player.getImageCache().get(mfr, packedFrame);
//
//				}
			}
			const auto& pOverlay = pTrackItr->second.pOverlay;
			if (pOverlay)
				pOverlay->render(context);
		}

		// displaying
		m_pWindow->glfwSwapBuffers();

		// updating time
		const auto elapsedMicroSeconds = metronom.tick();
		m_Player.offsetPlaybackTime(elapsedMicroSeconds);
		context.liveTime += Time(elapsedMicroSeconds.count(), 1000000);

		// handling input
		auto &keyStrokes = m_pWindow->getPendingKeys();
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
		running = !(m_pWindow->glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS && m_pWindow->glfwGetWindowParam(GLFW_FOCUSED));

		// dump info every seconds
		const auto now = duke_clock::now();
		if ((now - milestone) > std::chrono::seconds(1)) {
//				metronom.dump();
			milestone = now;
		}
	}
}
} /* namespace duke */
