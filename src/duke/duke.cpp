//#define TEST
#ifndef TEST

#include "TGA.h"

#include <duke/cmdline/CmdLineParameters.h>
#include <duke/time/Clock.h>
#include <duke/gl/GlFwApp.h>
#include <duke/gl/Mesh.hpp>
#include <duke/gl/Shader.hpp>
#include <duke/engine/Context.h>
#include <duke/engine/VolatileTexture.h>
#include <duke/engine/DukeWindow.h>
#include <duke/engine/Player.h>
#include <duke/engine/renderers/TextRenderer.h>
#include <duke/engine/TextOverlay.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glfw.h>

#include <iostream>
#include <sstream>

glm::mat4 getWorldViewProjActualPixel(const duke::Viewport viewport, const glm::ivec2 image, const glm::ivec2 pan, const int zoom) {
	using namespace glm;
	mediump_ivec2 translating; // translation must be integer to prevent aliasing
	translating += viewport.dimension; // moving to center
	translating /= 2; // moving to center
	translating += pan; // moving to center
	vec2 scaling(1);
	scaling /= 2; // bringing square from [-1,1] to [-.5,.5]
	scaling *= image; // to pixel dimension
	scaling *= (1 + (0.1 * zoom)); // zoom
	mat4 world;
	world = translate(world, vec3(translating, 0)); // move to center
	world = scale(world, vec3(scaling, 1));
	const mat4 proj = glm::ortho<float>(0, viewport.dimension.x, 0, viewport.dimension.y);
	return proj * world;
}
//
//std::ostream& operator<<(std::ostream& stream, const glm::ivec2 &value) {
//	return stream << '[' << value.x << ',' << value.y << ']';
//}
//
//std::ostream& operator<<(std::ostream& stream, const duke::Viewport &value) {
//	return stream << '(' << value.offset << ',' << value.dimension << ')';
//}

int main(int argc, char** argv) {
	using namespace std;
	using namespace duke;
	using namespace glm;
	try {
		const CmdLineParameters parameters(argc, argv);
		DukeWindow window;
		window.openWindow(512, 512, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);

		glfwSwapInterval(parameters.swapBufferInterval);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_DEPTH_TEST);

		// find textures here : http://dwarffortresswiki.org/index.php/Tileset_repository
//		TextRenderer textRenderer("Phoebus_16x16.png");
		TextRenderer textRenderer("Bisasam_24x24.png");
//		TextRenderer textRenderer("Phssthpok_16x16.png");

		TextOverlay textOverlay1(textRenderer, "INSERT");
		TextOverlay textOverlay2(textRenderer, "COINS\x2");

		Timeline timeline;
		timeline.resize(2);
		Track &firstTrack = timeline[0];
		firstTrack.add(0, Clip { 25, &textOverlay1 });
		firstTrack.add(25, Clip { 25, &textOverlay2 });
//		Track &secondTrack = timeline[1];
//		secondTrack.add(0, Clip { 200, &imageSequence });

		Player player;
		player.load(timeline, FrameDuration::PAL);
		player.setPlaybackSpeed(1);

		Program program( //
				loadVertexShader("shader/basic.vglsl"), //
				loadFragmentShader("shader/basic.fglsl"));
		const auto gWorldLocation = program.getUniformLocation("gWorld");
		const auto gTex0 = program.getUniformLocation("gTex0");
		const auto textureSampler = program.getUniformLocation("rectangleImageSampler");
		const auto pMesh = getSquare();

		// texture
		VolatileTexture texture(GL_TEXTURE_RECTANGLE);

		const GLenum textureMode = GL_NEAREST;
//		const GLenum textureMode = GL_LINEAR;
		const char* filename = "ScanLines/StillLife.exr";
//		const char* filename = "ScanLines/MtTamWest.exr";
//		const char* filename = "ScanLines/Tree.exr";
//		const char* filename = "test.tga";
//		const char* filename = "sample1920X1080dpx10bit.dpx";
//		const char* filename = "checker.png";
		texture.load(filename, textureMode, textureMode, GL_CLAMP_TO_EDGE);

		Context context;
		Metronom metronom(100);
		auto milestone = duke_clock::now();
		bool running = true;
		while (running) {
			glfwPollEvents();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			context.viewport = window.useViewport(false, false, false, false);
			context.currentFrame = player.getCurrentFrame();
			context.playbackTime = player.getPlaybackTime();

			// pass 1
			{
				program.use();
				const ivec2 image(texture.description.width, texture.description.height);
				const mat4 worldViewProj = getWorldViewProjActualPixel(context.viewport, image, window.getRelativeMousePos(), glfwGetMouseWheel());
				glUniformMatrix4fv(gWorldLocation, 1, GL_FALSE, value_ptr(worldViewProj));
				glUniform1i(textureSampler, 0);
				const auto scopeBind = texture.use(gTex0);
				pMesh->draw();
			}

			for (const Track &track : player.getTimeline()) {
				const MediaFrameReference mfr = track.getClipFrame(context.currentFrame.round());
				const Clip* pClip = mfr.first;
				if (!pClip)
					continue;
				const MediaStream *pStream = pClip->pStream;
				if (!pStream)
					continue;
				pStream->doRender(context);
			}

			glfwSwapBuffers();
			const auto elapsedMicroSeconds = metronom.tick();

			player.offsetPlaybackTime(elapsedMicroSeconds);

			auto &keyStrokes = window.getPendingKeys();
			for (const int key : keyStrokes) {
				switch (key) {
				case 'r':
					glfwSetMouseWheel(0);
					break;
				case 'g':
					gltWriteTGA("grab.tga");
					cout << "grabbed image" << endl;
					break;
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
	} catch (exception &e) {
		fprintf(stderr, "Unexpected error\n%s\n", e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
#endif // TEST
