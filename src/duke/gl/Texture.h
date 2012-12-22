/*
 * Texture.h
 *
 *  Created on: Dec 3, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <duke/gl/GLUtils.h>
#include <duke/gl/Buffer.hpp>

#include <GL/gl.h>

#include <stdexcept>

struct BufferTexturePolicy {
	inline static GLuint allocate() {
		GLuint id;
		glGenTextures(1, &id);
		checkError();
		return id;
	}
	inline static void deallocate(GLuint id) {
		glDeleteTextures(1, &id);
		checkError();
	}
	inline static void bind(GLuint targetType, GLuint id) {
		glBindTexture(targetType, id);
		checkError();
	}
	inline static void unbind(GLuint targetType) {
		bind(targetType, 0);
	}
	inline static GLuint checkTargetType(GLuint targetType) {
		switch (targetType) {
		case GL_TEXTURE_1D:
		case GL_TEXTURE_2D:
		case GL_TEXTURE_3D:
		case GL_TEXTURE_1D_ARRAY:
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_RECTANGLE:
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_CUBE_MAP_ARRAY:
		case GL_TEXTURE_BUFFER:
		case GL_TEXTURE_2D_MULTISAMPLE:
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			return targetType;
		default:
			throw std::runtime_error("invalid target texture type");
		}
	}
};

typedef Buffer<BufferTexturePolicy> TextureBuffer;

#include <memory>

typedef std::shared_ptr<TextureBuffer> SharedTextureBuffer;

#endif /* TEXTURE_H_ */
