/*
 * TextRenderer.h
 *
 *  Created on: Jan 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <duke/NonCopyable.h>
#include <duke/engine/DukeWindow.h>
#include <duke/engine/VolatileTexture.h>
#include <duke/gl/Shader.hpp>
#include <duke/gl/Mesh.hpp>

class TextRenderer: public NonCopyable {
public:
	TextRenderer(const char *glyphsFilename);
	void draw(const duke::Viewport &viewport, const char* pText);
private:
	VolatileTexture m_GlyphsTexture = (GL_TEXTURE_RECTANGLE);
	const Program m_TextProgram;
	const GLuint gViewport;
	const GLuint gImage;
	const GLuint gPan;
	const GLuint gChar;
	const GLuint gTextureSampler;
	const SharedMesh m_pMesh;
};

#endif /* TEXTRENDERER_H_ */
