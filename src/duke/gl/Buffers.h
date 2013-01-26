/*
 * Buffers.h
 *
 *  Created on: Jan 17, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef BUFFERS_H_
#define BUFFERS_H_

#include <duke/NonCopyable.h>
#include <duke/gl/GL.h>
#include <duke/gl/GLUtils.h>
#include <stdexcept>
#include <climits>

namespace duke {

template<class T>
struct Binder: public noncopyable {
	Binder(Binder&& other) : unbind(true) { other.unbind = false; }
	Binder(GLuint id) : unbind(true) {T::bind(T::TargetType, id); glCheckError();}
	~Binder() {glCheckError(); if(unbind) T::bind(T::TargetType, 0);}
	bool unbind;
};

template<class T>
Binder<T> scope_bind(const T& buffer) {
	return {buffer.id};
}

namespace details {

struct GlBufferAllocactor {
	inline static GLuint allocate() {
		GLuint id;
		glGenBuffers(1, &id);
		return id;
	}
	inline static void deallocate(const GLuint id) {
		glDeleteBuffers(1, &id);
	}
	inline static void bind(const GLuint target, const GLuint id) {
		glBindBuffer(target, id);
	}
};

template<typename ALLOC, GLuint TARGETTYPE = UINT_MAX>
struct GlObject: public noncopyable, public ALLOC {
	enum {
		TargetType = TARGETTYPE
	};
	GlObject() :
			id(ALLOC::allocate()) {
		glCheckError();
	}
	~GlObject() {
		ALLOC::deallocate(id);
		glCheckError();
	}

	const GLuint id;
};

template<GLuint TARGETTYPE, GLuint USAGE>
struct GlBufferObject: public GlObject<GlBufferAllocactor, TARGETTYPE> {
	static_assert(
			USAGE==GL_STREAM_DRAW||USAGE==GL_STREAM_READ||USAGE==GL_STREAM_COPY|| //
			USAGE==GL_STATIC_DRAW||USAGE==GL_STATIC_READ||USAGE==GL_STATIC_COPY||//
			USAGE==GL_DYNAMIC_DRAW||USAGE==GL_DYNAMIC_READ||USAGE==GL_DYNAMIC_COPY, "Unsupported usage");
	inline void bufferData(GLsizeiptr size, const GLvoid * data) const {
		glCheckBound(TARGETTYPE, this->id);
		glBufferData(TARGETTYPE, size, data, USAGE);
		glCheckError();
	}
};

template<GLuint TARGETTYPE, GLuint USAGE>
struct VBO: public GlBufferObject<TARGETTYPE, USAGE> {
	static_assert(TARGETTYPE==GL_ARRAY_BUFFER||TARGETTYPE==GL_ELEMENT_ARRAY_BUFFER, "Unsupported target type");
};

template<GLuint TARGETTYPE, GLuint USAGE>
struct PBO: public GlBufferObject<TARGETTYPE, USAGE> {
	static_assert(TARGETTYPE==GL_PIXEL_PACK_BUFFER||TARGETTYPE==GL_PIXEL_UNPACK_BUFFER, "Unsupported target type");
};

struct GlVertexArrayAllocator {
	inline static GLuint allocate() {
		GLuint id;
		glGenVertexArrays(1, &id);
		return id;
	}
	inline static void deallocate(const GLuint id) {
		glDeleteVertexArrays(1, &id);
	}
	inline static void bind(const GLuint target, const GLuint id) {
		glBindVertexArray(id);
	}
};

struct VAO: public GlObject<GlVertexArrayAllocator> {
};

}  // namespace details

typedef details::VBO<GL_ARRAY_BUFFER, GL_STATIC_DRAW> StaticVbo;
typedef details::VBO<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> StaticIndexedVbo;
typedef details::VAO Vao;
typedef details::PBO<GL_PIXEL_UNPACK_BUFFER, GL_STREAM_DRAW> StreamUploadPbo;
typedef details::PBO<GL_PIXEL_UNPACK_BUFFER, GL_STATIC_DRAW> StaticUploadPbo;

}  // namespace duke

#endif /* BUFFERS_H_ */
