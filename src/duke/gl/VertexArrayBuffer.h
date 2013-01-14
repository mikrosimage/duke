/*
 * VertexArrayBuffer.h
 *
 *  Created on: Jan 14, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef VERTEXARRAYBUFFER_H_
#define VERTEXARRAYBUFFER_H_

#include <duke/gl/GL.h>

struct VertexArrayBufferPolicy {
	inline static GLuint allocate() {
		GLuint id;
		glGenVertexArrays(1, &id);
		checkError();
		return id;
	}
	inline static void deallocate(GLuint id) {
		glDeleteVertexArrays(1, &id);
		checkError();
	}
	inline static void bind(GLuint targetType, GLuint id) {
		glBindVertexArray(id);
		checkError();
	}
	inline static void unbind(GLuint targetType) {
		bind(targetType, 0);
	}
	inline static GLuint checkTargetType(GLuint targetType) {
		return 0;
	}
};

struct VertexArrayBuffer: public Buffer<VertexArrayBufferPolicy> {
	VertexArrayBuffer() :
			Buffer<VertexArrayBufferPolicy>(0) {
	}
};

#endif /* VERTEXARRAYBUFFER_H_ */
