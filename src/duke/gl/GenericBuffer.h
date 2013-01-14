/*
 * GenericBuffer.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef GENERICBUFFER_H_
#define GENERICBUFFER_H_

#include <duke/gl/Buffer.hpp>
#include <duke/gl/GLUtils.h>
#include <duke/gl/GL.h>

#include <stdexcept>

struct BufferGenericPolicy {
	inline static GLuint allocate() {
		GLuint id;
		glGenBuffers(1, &id);
		checkError();
		return id;
	}
	inline static void deallocate(GLuint id) {
		glDeleteBuffers(1, &id);
		checkError();
	}
	inline static void bind(GLuint targetType, GLuint id) {
		glBindBuffer(targetType, id);
		checkError();
	}
	inline static void unbind(GLuint targetType) {
		bind(targetType, 0);
	}
	inline static GLuint checkTargetType(GLuint targetType) {
		switch (targetType) {
		case GL_ARRAY_BUFFER:
		case GL_ELEMENT_ARRAY_BUFFER:
		case GL_ATOMIC_COUNTER_BUFFER:
		case GL_COPY_READ_BUFFER:
		case GL_COPY_WRITE_BUFFER:
		case GL_DRAW_INDIRECT_BUFFER:
		case GL_PIXEL_PACK_BUFFER:
		case GL_PIXEL_UNPACK_BUFFER:
		case GL_TEXTURE_BUFFER:
		case GL_TRANSFORM_FEEDBACK_BUFFER:
		case GL_UNIFORM_BUFFER:
			return targetType;
		default:
			throw std::runtime_error("invalid target buffer type");
		}
	}
};

typedef Buffer<BufferGenericPolicy> GenericBuffer;

#endif /* GENERICBUFFER_H_ */
