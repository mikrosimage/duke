/*
 * Textures.h
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTURES_H_
#define TEXTURES_H_

#include <duke/gl/Buffers.h>
#include <duke/imageio/ImageDescription.h>
#include <duke/engine/ImageLoadUtils.h>

namespace duke {

class ITexture {
public:
	virtual ~ITexture() {
	}
	void initialize(const PackedFrameDescription &description, const GLvoid * data = nullptr) {
		const auto packFormat = description.glPackFormat;
		initialize(description, getAdaptedInternalFormat(packFormat), getPixelFormat(packFormat), getPixelType(packFormat), data);
	}
	virtual void initialize(const PackedFrameDescription &description, GLint internalFormat, GLenum format, GLenum type, const GLvoid * data) =0;
	virtual void update(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * data) =0;
	virtual void doBind(bool) const =0;
	PackedFrameDescription description;
};

template<>
struct Binder<ITexture> : public noncopyable {
	Binder(Binder&&other) : m_pTexture(other.m_pTexture) {}
	Binder(const ITexture *pTexture) : m_pTexture(pTexture) {pTexture->doBind(true); glCheckError();}
	~Binder() {glCheckError(); m_pTexture->doBind(false);}
	const ITexture * m_pTexture;
};

template<>
inline Binder<ITexture> scope_bind(const ITexture& buffer) {
	return {&buffer};
}

namespace details {

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
struct Texture: public GlObject<GlTextureAllocator, TARGETTYPE>, public ITexture {
	static_assert(TARGETTYPE==GL_TEXTURE_2D||TARGETTYPE==GL_TEXTURE_RECTANGLE, "Unsupported target type");
	virtual void initialize(const PackedFrameDescription &description, GLint internalFormat, GLenum format, GLenum type, const GLvoid * data) {
		glCheckBound(TARGETTYPE, this->id);
		printf("about to glTexImage2D, original internal %s, internal %s, pixel format %s, pixel type %s\n", //
				getInternalFormatString(description.glPackFormat), //
				getInternalFormatString(internalFormat), //
				getPixelFormatString(format), //
				getPixelTypeString(type));
		glTexImage2D(TARGETTYPE, 0, internalFormat, description.width, description.height, 0, format, type, data);
		glCheckError();
		this->description = description;
	}
	virtual void update(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * data) {
		glCheckBound(TARGETTYPE, this->id);
		glTexSubImage2D(TARGETTYPE, 0, xoffset, yoffset, width, height, format, type, data);
		glCheckError();
	}
	virtual void doBind(bool doBind) const {
		GlTextureAllocator::bind(TARGETTYPE, doBind ? this->id : 0);
	}
};

}  // namespace details

typedef details::Texture<GL_TEXTURE_2D> Texture2D;
typedef details::Texture<GL_TEXTURE_RECTANGLE> TextureRectangle;

}  // namespace duke

#endif /* TEXTURES_H_ */
