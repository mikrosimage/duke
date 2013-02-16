/*
 * Textures.h
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTURES_H_
#define TEXTURES_H_

#include <duke/gl/GlObjects.h>
#include <duke/imageio/PackedFrameDescription.h>

namespace duke {

struct Texture: public gl::GlTextureRectangle {
	void initialize(const PackedFrameDescription &description, const GLvoid * pData = nullptr);
	void initialize(const PackedFrameDescription &description, GLint internalFormat, GLenum format, GLenum type, const GLvoid * pData);
	void update(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pData);

	PackedFrameDescription description;
};

} // namespace duke

#endif /* TEXTURES_H_ */
