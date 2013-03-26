#pragma once

#include <memory>
#include <duke/NonCopyable.hpp>
#include <duke/gl/GL.hpp>
#include <duke/gl/GlObjectsDetails.hpp>

namespace duke {
namespace gl {

class IBindable: public noncopyable {
public:
	virtual ~IBindable() {
	}
	virtual void bind() const=0;
	virtual void unbind() const=0;
};

class GlObject: public IBindable {
public:
	GlObject(GLuint id);

	Binder<GlObject> scope_bind() const { return {this}; }

	const GLuint id;
};

class GlVertexArrayObject: public GlObject {
public:
	GlVertexArrayObject();
	virtual ~GlVertexArrayObject();
	virtual void bind() const;
	virtual void unbind() const;
};

class GlTextureObject: public GlObject {
public:
	GlTextureObject(GLenum target);
	virtual ~GlTextureObject();
	virtual void bind() const;
	virtual void unbind() const;

	Binder<GlTextureObject> scope_bind_texture() const { return {this}; }

	const GLenum target;
};

struct GlTexture2D: public GlTextureObject {
	GlTexture2D();
};

struct GlTextureRectangle: public GlTextureObject {
	GlTextureRectangle();
};

class GlBufferObject: public GlObject {
public:
	GlBufferObject(GLenum target, GLenum usage);
	virtual ~GlBufferObject();

	virtual void bind() const;
	virtual void unbind() const;

	inline Binder<GlBufferObject> scope_bind_buffer() const { return {this}; }

	const GLenum target;
	const GLenum usage;
};

struct GlStaticVbo: public GlBufferObject {
	GlStaticVbo();
};

struct GlStaticIndexedVbo: public GlBufferObject {
	GlStaticIndexedVbo();
};

struct GlStreamUploadPbo: public GlBufferObject {
	GlStreamUploadPbo();
};

struct GlStaticUploadPbo: public GlBufferObject {
	GlStaticUploadPbo();
};

} /* namespace gl */
} /* namespace duke */
