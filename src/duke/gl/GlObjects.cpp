#include "GlObjects.hpp"
#include <duke/gl/GL.hpp>
#include <duke/gl/GLUtils.hpp>

namespace duke {
namespace gl {

GlObject::GlObject(GLuint id) : id(id) {}

namespace {

GLuint allocateVertexArrayObject() {
  GLuint id;
  glGenVertexArrays(1, &id);
  return id;
}

}  // namespace

GlVertexArrayObject::GlVertexArrayObject() : GlObject(allocateVertexArrayObject()) {}
GlVertexArrayObject::~GlVertexArrayObject() { glDeleteVertexArrays(1, &id); }
void GlVertexArrayObject::bind() const { glBindVertexArray(id); }
void GlVertexArrayObject::unbind() const { glBindVertexArray(0); }

namespace {

GLuint allocateTextureObject() {
  GLuint id;
  glGenTextures(1, &id);
  return id;
}

}  // namespace

GlTextureObject::GlTextureObject(GLenum target) : GlObject(allocateTextureObject()), target(target) {}
GlTextureObject::~GlTextureObject() { glDeleteTextures(1, &id); }
void GlTextureObject::bind() const { glBindTexture(target, id); }
void GlTextureObject::unbind() const { glBindTexture(target, 0); }

GlTexture2D::GlTexture2D() : GlTextureObject(GL_TEXTURE_2D) {}

GlTextureRectangle::GlTextureRectangle() : GlTextureObject(GL_TEXTURE_RECTANGLE) {}

namespace {

GLuint allocateBufferObject() {
  GLuint id;
  glGenBuffers(1, &id);
  return id;
}

}  // namespace

GlBufferObject::GlBufferObject(GLenum target, GLenum usage)
    : GlObject(allocateBufferObject()), target(target), usage(usage) {}
GlBufferObject::~GlBufferObject() { glDeleteBuffers(1, &id); }
void GlBufferObject::bind() const { glBindBuffer(target, id); }
void GlBufferObject::unbind() const { glBindBuffer(target, 0); }

GlStaticVbo::GlStaticVbo() : GlBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW) {}

GlStaticIndexedVbo::GlStaticIndexedVbo() : GlBufferObject(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW) {}

GlStreamUploadPbo::GlStreamUploadPbo() : GlBufferObject(GL_PIXEL_UNPACK_BUFFER, GL_STREAM_DRAW) {}

GlStaticUploadPbo::GlStaticUploadPbo() : GlBufferObject(GL_PIXEL_UNPACK_BUFFER, GL_STATIC_DRAW) {}

} /* namespace gl */
} /* namespace duke */
