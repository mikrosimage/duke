/*
 * GlObjects.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: Guillaume Chatelet
 */

#include "GlObjects.h"
#include <duke/gl/GL.h>
#include <duke/gl/GLUtils.h>

namespace duke {
namespace gl {

GlObject::GlObject(GLuint id) :
		id(id) {
}
void GlObject::boundDo(const std::function<void()> &func) const {
	auto bound = scope_bind();
	func();
}

inline static GLuint allocateVertexArrayObject() {
	GLuint id;
	glGenVertexArrays(1, &id);
	return id;
}

GlVertexArrayObject::GlVertexArrayObject() :
		GlObject(allocateVertexArrayObject()) {
}
GlVertexArrayObject::~GlVertexArrayObject() {
	glDeleteVertexArrays(1, &id);
}
void GlVertexArrayObject::bind() const {
	glBindVertexArray(id);
}
void GlVertexArrayObject::unbind() const {
	glBindVertexArray(0);
}

inline static GLuint allocateTextureObject() {
	GLuint id;
	glGenTextures(1, &id);
	return id;
}

GlTextureObject::GlTextureObject(GLenum target) :
		GlObject(allocateTextureObject()), target(target) {
}
GlTextureObject::~GlTextureObject() {
	glDeleteTextures(1, &id);
}
void GlTextureObject::bind() const {
	glBindTexture(target, id);
}
void GlTextureObject::unbind() const {
	glBindTexture(target, 0);
}
void GlTextureObject::boundDo(const std::function<void(const GLenum target)> &func) const {
	auto bound = scope_bind_texture();
	func(target);
}

GlTexture2D::GlTexture2D() :
		GlTextureObject(GL_TEXTURE_2D) {
}

GlTextureRectangle::GlTextureRectangle() :
		GlTextureObject(GL_TEXTURE_RECTANGLE) {
}

inline static GLuint allocateBufferObject() {
	GLuint id;
	glGenBuffers(1, &id);
	return id;
}

GlBufferObject::GlBufferObject(GLenum target, GLenum usage) :
		GlObject(allocateBufferObject()), target(target), usage(usage) {
}
GlBufferObject::~GlBufferObject() {
	glDeleteBuffers(1, &id);
}
void GlBufferObject::bind() const {
	glBindBuffer(target, id);
}
void GlBufferObject::unbind() const {
	glBindBuffer(target, 0);
}
void GlBufferObject::boundDo(const std::function<void(const GLenum target, const GLenum usage)> &func) const {
	auto bound = scope_bind_buffer();
	func(target, usage);
}

GlStaticVbo::GlStaticVbo() :
		GlBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW) {
}

GlStaticIndexedVbo::GlStaticIndexedVbo() :
		GlBufferObject(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW) {
}

GlStreamUploadPbo::GlStreamUploadPbo() :
		GlBufferObject(GL_PIXEL_UNPACK_BUFFER, GL_STREAM_DRAW) {
}

GlStaticUploadPbo::GlStaticUploadPbo() :
		GlBufferObject(GL_PIXEL_UNPACK_BUFFER, GL_STATIC_DRAW) {
}

} /* namespace gl */
} /* namespace duke */
