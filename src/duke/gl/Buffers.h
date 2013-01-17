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

template<class T>
struct Binder: public noncopyable {
	Binder(Binder&&) {}
	Binder(GLuint id) {T::bind(T::TargetType, id); glCheckError();}
	~Binder() {glCheckError(); T::bind(T::TargetType, 0);}
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

template<typename ALLOC, GLuint TARGETTYPE = -1>
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

struct GlTextureAllocator {
	inline static GLuint allocate() {
		GLuint id;
		glGenTextures(1, &id);
		return id;
	}
	inline static void deallocate(const GLuint id) {
		glDeleteTextures(1, &id);
	}
	inline static void bind(const GLuint target, const GLuint id) {
		glBindTexture(target, id);
	}
};

template<GLuint TARGETTYPE>
struct Texture: public GlObject<GlTextureAllocator, TARGETTYPE> {
	static_assert(TARGETTYPE==GL_TEXTURE_2D||TARGETTYPE==GL_TEXTURE_RECTANGLE, "Unsupported target type");
	void texSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * data) {
		glTexSubImage2D(TARGETTYPE, level, xoffset, yoffset, width, height, format, type, data);
		glCheckError();
	}
	void texImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data) {
		glTexImage2D(TARGETTYPE, level, internalFormat, width, height, border, format, type, data);
		glCheckError();
	}
};

}  // namespace details

typedef details::VBO<GL_ARRAY_BUFFER, GL_STATIC_DRAW> StaticVbo;
typedef details::VBO<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> StaticIndexedVbo;
typedef details::VAO Vao;
typedef details::PBO<GL_PIXEL_UNPACK_BUFFER, GL_STREAM_DRAW> StreamUploadPbo;
typedef details::PBO<GL_PIXEL_UNPACK_BUFFER, GL_STATIC_DRAW> StaticUploadPbo;
typedef details::Texture<GL_TEXTURE_2D> Texture2D;
typedef details::Texture<GL_TEXTURE_RECTANGLE> TextureRectangle;

#endif /* BUFFERS_H_ */
