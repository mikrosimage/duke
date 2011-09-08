/*
 * OGLTexture.h
 *
 *  Created on: 02 juin 2010
 *      Author: Nicolas Rondaud
 */

#ifndef OGLTEXTURE_H_
#define OGLTEXTURE_H_

#include <GL/glew.h>
#include <plugin/math/Rect.h>
#include <plugin/ITextureBase.h>

class OGLRenderer;

class OGLTexture : public ITextureBase
{
public:
	OGLTexture( const ImageDescription& description, unsigned long usageFlag, const OGLRenderer& renderer );
	~OGLTexture();

	inline unsigned int getGLTexture() const
	{
		return m_Texture;
	}

private:
	virtual void update( const ImageDescription& description, const unsigned char* pData );
	GLuint m_Texture;
	GLuint m_Pbo;
	bool m_bUsePBO;
};

#endif /* OGLTEXTURE_H_ */
