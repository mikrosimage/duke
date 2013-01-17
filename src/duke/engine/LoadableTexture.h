/*
 * LoadableTexture.h
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef LOADABLETEXTURE_H_
#define LOADABLETEXTURE_H_

#include <duke/imageio/DukeIO.h>
#include <duke/imageio/Attributes.h>
#include <duke/gl/Buffers.h>

class IImageReader;
// forward decl

struct LoadableTexture {
	LoadableTexture();
	bool load(const char* filename, GLuint minFilter, GLuint magFilter, GLuint wrapMode);
	Binder<TextureRectangle> use(GLuint dimensionUniformParameter) const;
	Attributes attributes;
	ImageDescription description;
	GLuint minFilter;
	GLuint magFilter;
	GLuint wrapMode;
private:
	std::string loadImage(IImageReader *);
	std::string tryReader(const char* filename, const IIODescriptor *pDescriptor);
	void loadGlTexture(const void* pData);
	std::shared_ptr<TextureRectangle> m_pTextureBuffer;
};

#endif /* LOADABLETEXTURE_H_ */
