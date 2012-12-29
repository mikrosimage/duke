#define TEST
#ifndef TEST

#include "TGA.h"

#include <duke/cmdline/CmdLineParameters.h>
#include <duke/time/Clock.h>
#include <duke/gl/GlFwApp.h>
#include <duke/gl/Mesh.hpp>
#include <duke/gl/Shader.hpp>
#include <duke/engine/VolatileTexture.h>
#include <duke/engine/DukeWindow.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glfw.h>

#include <iostream>

SharedMesh getSquare() {
	using namespace std;
	const float z = 1;
	const vector<VertexPosUv0> vertices = { //
			{ glm::vec3(-1, -1, z), glm::vec2(0, 0) }, //
					{ glm::vec3(-1, 1, z), glm::vec2(0, 1) }, //
					{ glm::vec3(1, 1, z), glm::vec2(1, 1) }, //
					{ glm::vec3(1, -1, z), glm::vec2(1, 0) } };
	return make_shared<Mesh>(GL_TRIANGLE_FAN, vertices.data(), vertices.size());
}

glm::mat4 getWorldViewProjActualPixel(const duke::Viewport viewport, const glm::vec2 image, const glm::vec2 pan, const int zoom) {
	using namespace glm;
	vec2 translating;
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

std::ostream& operator<<(std::ostream& stream, const glm::vec2 &value) {
	return stream << '[' << value.x << ',' << value.y << ']';
}

std::ostream& operator<<(std::ostream& stream, const duke::Viewport &value) {
	return stream << '(' << value.offset << ',' << value.dimension << ')';
}

int main(int argc, char** argv) {
	using namespace std;
	using namespace duke;
	using namespace glm;
	try {
		const CmdLineParameters parameters(argc, argv);
		DukeWindow window;
		window.openWindow(512, 512, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);

		glfwSwapInterval(parameters.swapBufferInterval);
		glEnable(GL_DEPTH_TEST);

		Program program( //
				loadVertexShader("shader/basic.vglsl"), //
				loadFragmentShader("shader/basic.fglsl"));
		const auto gWorldLocation = program.getUniformLocation("gWorld");
		const auto locRectTexture = program.getUniformLocation("rectangleImage");
		const auto gTex0 = program.getUniformLocation("gTex0");
		const auto pMesh = getSquare();

		// texture
		VolatileTexture texture(GL_TEXTURE_RECTANGLE);

		const GLenum textureMode = GL_LINEAR;
//		texture.load("ScanLines/MtTamWest.exr", textureMode, textureMode, GL_CLAMP_TO_EDGE);
//		texture.load("ScanLines/Tree.exr", textureMode, textureMode, GL_CLAMP_TO_EDGE);
//		texture.load("ScanLines/StillLife.exr", textureMode, textureMode, GL_CLAMP_TO_EDGE);
//		texture.load("test.tga", textureMode, textureMode, GL_CLAMP_TO_EDGE);
//		texture.load("sample1920X1080dpx10bit.dpx", textureMode, textureMode, GL_CLAMP_TO_EDGE);
		texture.load("checker.png", textureMode, textureMode, GL_CLAMP_TO_EDGE);

		Metronom metronom(100);
		auto milestone = std::chrono::steady_clock::now();
		bool running = true;
		while (running) {
			glfwPollEvents();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			program.use();

//			const float seconds = (uint64_t(clock_utils::getRealtimeMicroSec().us) & 0xFFFFFFFF) / 1000000.;
//			const float roundPerSeonds = seconds * 360;

			const vec2 image(texture.description.width, texture.description.height);
			const auto viewport = window.useViewport(false, false, false, false);
			const mat4 worldViewProj = getWorldViewProjActualPixel(viewport, image, window.getRelativeMousePos(), glfwGetMouseWheel());
			glUniformMatrix4fv(gWorldLocation, 1, GL_FALSE, value_ptr(worldViewProj));

			glUniform1i(locRectTexture, 0);
			const auto scopeBind = texture.use(gTex0);

			pMesh->draw();

			glfwSwapBuffers();
			metronom.tick();

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
			const auto now = std::chrono::steady_clock::now();
			if ((now - milestone) > std::chrono::seconds(1)) {
				metronom.dump();
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
