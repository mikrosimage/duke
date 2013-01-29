/*
 * Benchmark.cpp
 *
 *  Created on: Jan 28, 2013
 *      Author: Guillaume Chatelet
 */

#include "Benchmark.h"

#include <duke/gl/GlFwApp.h>
#include <duke/gl/GL.h>
#include <duke/gl/Textures.h>

#include <glm/glm.hpp>

#include <chrono>
#include <vector>
#include <map>
#include <memory>

using namespace std;

namespace duke {

struct TextureConfiguration {
	GLuint internalFormat;
	GLenum pixel_format;
	GLenum pixel_type;
};

void benchmark() {
	DukeGLFWApplication application;
	unique_ptr<DukeGLFWWindow> pWindow(application.createWindow<DukeGLFWWindow>(100, 100, "please wait while benchmarking...", nullptr, nullptr));
	const char *pData = new char[100 * 1024 * 1024]; //allocating 100 MB

	const vector<glm::uvec2> textureSizes = { glm::uvec2(1920, 1080), glm::uvec2(2048, 1536), glm::uvec2(4096, 3072) };
	const vector<TextureConfiguration> configurations = { //
			{ GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8 }, //
			{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 }, //
			{ GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV }, //
			{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV }, //
			{ GL_RGB, GL_RGB, GL_UNSIGNED_BYTE }, //
			{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE }, //
			{ GL_RGB, GL_BGR, GL_UNSIGNED_BYTE }, //
			{ GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE }, //
			{ GL_RGB, GL_RGB, GL_UNSIGNED_SHORT }, //
			{ GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT }, //
			{ GL_RGB, GL_BGR, GL_UNSIGNED_SHORT }, //
			{ GL_RGBA, GL_BGRA, GL_UNSIGNED_SHORT }, //
			{ GL_RGB, GL_RGB, GL_HALF_FLOAT }, //
			{ GL_RGBA, GL_RGBA, GL_HALF_FLOAT }, //
			{ GL_RGB, GL_BGR, GL_HALF_FLOAT }, //
			{ GL_RGBA, GL_BGRA, GL_HALF_FLOAT }, //
			};
	for (glm::uvec2 textureSize : textureSizes) {
		for (TextureConfiguration conf : configurations) {
			const GLuint id = details::GlTextureAllocator::allocate();
			details::GlTextureAllocator::bind(GL_TEXTURE_RECTANGLE, id);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, conf.internalFormat, textureSize.x, textureSize.y, 0, conf.pixel_format, conf.pixel_type, nullptr);
			const auto start = chrono::steady_clock::now();
			size_t count = 0;
			for (; chrono::steady_clock::now() - start < chrono::seconds(2); ++count)
				glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, textureSize.x, textureSize.y, conf.pixel_format, conf.pixel_type, pData);
			details::GlTextureAllocator::deallocate(id);
			printf("%u,%u %10s %10s %30s %ld\n", //
					textureSize.x, textureSize.y, //
					getInternalFormatString(conf.internalFormat), //
					getPixelFormatString(conf.pixel_format), //
					getPixelTypeString(conf.pixel_type), //
					count);
		}
	}
	delete[] pData;
}

} /* namespace duke */
