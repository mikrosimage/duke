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

static size_t getChannelCount(GLenum pixel_format) {
	switch (pixel_format) {
	case GL_RGBA:
	case GL_BGRA:
		return 4;
	case GL_RGB:
	case GL_BGR:
		return 3;
	default:
		throw runtime_error("channel count not implemented");
	}
}

static size_t getBytePerChannel(GLenum pixel_type) {
	switch (pixel_type) {
	case GL_UNSIGNED_INT_8_8_8_8:
	case GL_UNSIGNED_INT_8_8_8_8_REV:
	case GL_UNSIGNED_BYTE:
		return 1;
	case GL_UNSIGNED_SHORT:
	case GL_HALF_FLOAT:
		return 2;
	case GL_FLOAT:
	case GL_INT:
		return 4;
	default:
		throw runtime_error("byte per channel not implemented");
	}
}

static size_t getBytePerPixels(GLenum pixel_format, GLenum pixel_type) {
	return getChannelCount(pixel_format) * getBytePerChannel(pixel_type);
}

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
					{ GL_RGB, GL_BGR, GL_UNSIGNED_BYTE }, //
					{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE }, //
					{ GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE }, //
					{ GL_RGB, GL_RGB, GL_HALF_FLOAT }, //
					{ GL_RGB, GL_BGR, GL_HALF_FLOAT }, //
					{ GL_RGB16F, GL_RGB, GL_HALF_FLOAT }, //
					{ GL_RGB16F, GL_BGR, GL_HALF_FLOAT }, //
					{ GL_RGBA, GL_RGBA, GL_HALF_FLOAT }, //
					{ GL_RGBA, GL_BGRA, GL_HALF_FLOAT }, //
					{ GL_RGBA16, GL_RGBA, GL_HALF_FLOAT }, //
					{ GL_RGBA16, GL_BGRA, GL_HALF_FLOAT }, //
					{ GL_RGBA16F, GL_BGRA, GL_HALF_FLOAT }, //
					{ GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT }, //
			};
	const size_t seconds = 1;
	for (glm::uvec2 textureSize : textureSizes) {
		for (TextureConfiguration conf : configurations) {
			const GLuint id = details::GlTextureAllocator::allocate();
			details::GlTextureAllocator::bind(GL_TEXTURE_RECTANGLE, id);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, conf.internalFormat, textureSize.x, textureSize.y, 0, conf.pixel_format, conf.pixel_type, nullptr);
			const auto start = chrono::steady_clock::now();
			size_t count = 0;
			for (; chrono::steady_clock::now() - start < chrono::seconds(seconds); ++count)
				glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, textureSize.x, textureSize.y, conf.pixel_format, conf.pixel_type, pData);
			details::GlTextureAllocator::deallocate(id);
			const double gbPerSeconds = count * textureSize.x * textureSize.y * getBytePerPixels(conf.pixel_format, conf.pixel_type) / 1000. / 1000 / 1000 / seconds;
			printf("%u,%u %15s %10s %30s %3.0f FPS ( %3.2f GiB/s ) %3.2f ms\n", //
					textureSize.x, textureSize.y, //
					getInternalFormatString(conf.internalFormat), //
					getPixelFormatString(conf.pixel_format), //
					getPixelTypeString(conf.pixel_type), //
					count / double(seconds), //
					gbPerSeconds, //
					1000 * seconds / double(count));
		}
	}
	delete[] pData;
}

} /* namespace duke */
