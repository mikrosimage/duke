#include "TGA.h"

#include <duke/memory/alloc/Allocator.h>
#include <duke/io/MemoryMappedFile.h>
#include <duke/imageio/DukeIO.h>
#include <duke/cmdline/CmdLineParameters.h>
#include <duke/clock/Clock.h>
#include <duke/gl/GlFwApp.h>
#include <duke/gl/Buffer.hpp>
#include <duke/gl/Mesh.hpp>
#include <duke/gl/Shader.hpp>
#include <duke/gl/Texture.h>
#include <duke/DukeWindow.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glfw.h>

#include <set>
#include <vector>
#include <stdexcept>
#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cmath>

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

GLint getInternalFormat(GLint format) {
	switch (format) {
	case GL_BGR:
		return GL_RGB;
	case GL_BGRA:
		return GL_RGBA;
	default:
		return format;
	}
}

void loadGlTexture(const ImageDescription& desc, const void* pData, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, magFilter);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, getInternalFormat(desc.glFormat), desc.width, desc.height, 0, desc.glFormat, desc.glType, pData);
	printf("loaded %ldx%ld", desc.width, desc.height);
	checkError();
}

AlignedMalloc alignedMalloc;

std::string loadImage(std::unique_ptr<IImageReader> &&pReader, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {
	if (!pReader) {
		return std::string("bad state : IImageReader==nullptr");
	}
	if (pReader->hasError()) {
		return pReader->getError();
	}
	const auto desc = pReader->getDescription();
	const void* pMapped = pReader->getMappedImageData();
	if (pMapped==nullptr) {
		const auto pData = make_shared_memory<char>(desc.dataSize, alignedMalloc);
		pReader->readImageDataTo(pData.get());
		if (pReader->hasError()) {
			return pReader->getError();
		}
		loadGlTexture(desc, pData.get(), minFilter, magFilter, wrapMode);
	} else {
		loadGlTexture(desc, pMapped, minFilter, magFilter, wrapMode);
	}
	return std::string();
}

bool loadImage(const char* filename, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {
	const char* pDot = strrchr(filename, '.');
	if (!pDot)
		return false;
	std::string error;
	for (const IIODescriptor *pDescriptor : IODescriptors::instance().findDescriptor(++pDot)) {
		std::unique_ptr<MemoryMappedFile> pMapped;
		std::unique_ptr<IImageReader> pReader;
		if (pDescriptor->supports(IIODescriptor::Capability::READER_READ_FROM_MEMORY)) {
			pMapped.reset(new MemoryMappedFile(filename)); // bad locality
			if (!(*pMapped)) {
				error = "unable to map file to memory";
				continue;
			}
			pReader.reset(pDescriptor->getReaderFromMemory(pMapped->pFileData, pMapped->fileSize));
		} else
			pReader.reset(pDescriptor->getReaderFromFile(filename));
		error = loadImage(std::move(pReader), minFilter, magFilter, wrapMode);
		if (error.empty())
			return true;
	}
	printf("error while reading %s : %s\n", filename, error.c_str());
	return false;
}

struct Texture {
	ImageDescription description;
	Texture(const char* pFilename) {

	}
};

#if true
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
		const auto pMesh = getSquare();

		// texture
		const auto pTextureBuffer = std::make_shared<TextureBuffer>(GL_TEXTURE_RECTANGLE);

		///////////////////////////////////////////
		// Load the main texture
		{
			ScopeBinder<TextureBuffer> scopeBind(pTextureBuffer);
			loadImage("test.tga", GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);
//			loadImage("sample1920X1080dpx10bit.dpx", GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);
		}

		Metronom metronom(100);
		auto milestone = std::chrono::steady_clock::now();
		bool running = true;
		while (running) {
			glfwPollEvents();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//			const auto render = rendering(program, pMesh, pTexture);

			program.use();

//			const float seconds = (uint64_t(clock_utils::getRealtimeMicroSec().us) & 0xFFFFFFFF) / 1000000.;
//			const float roundPerSeonds = seconds * 360;

			const vec2 image(1920, 1200);
			const auto viewport = window.useViewport(false, false, false, false);
			const mat4 worldViewProj = getWorldViewProjActualPixel(viewport, image, window.getRelativeMousePos(), glfwGetMouseWheel());
			glUniformMatrix4fv(gWorldLocation, 1, GL_FALSE, value_ptr(worldViewProj));

			glUniform1i(locRectTexture, 0);
			ScopeBinder<TextureBuffer> scopeBind(pTextureBuffer);

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
#endif
