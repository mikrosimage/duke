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
#include <duke/gl/Texture.h>

class IImageReader;
// forward decl

struct LoadableTexture {
	LoadableTexture(GLuint type);
	bool load(const char* filename, GLuint minFilter, GLuint magFilter, GLuint wrapMode);
	ScopeBinder<TextureBuffer> use(GLuint dimensionUniformParameter) const;
	Attributes attributes;
	ImageDescription description;
	GLuint minFilter;
	GLuint magFilter;
	GLuint wrapMode;
	GLuint textureType;
private:
	std::string loadImage(IImageReader *);
	std::string tryReader(const char* filename, const IIODescriptor *pDescriptor);
	void loadGlTexture(const void* pData);
	SharedTextureBuffer m_pTextureBuffer;
};

#endif /* LOADABLETEXTURE_H_ */
