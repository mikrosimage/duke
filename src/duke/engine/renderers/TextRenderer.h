/*
 * TextRenderer.h
 *
 *  Created on: Jan 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <duke/gl/Textures.h>
#include <duke/gl/Shader.hpp>
#include <duke/gl/Mesh.hpp>
#include <duke/imageio/Attributes.h>

namespace duke {

struct ITexture;
struct TextureDescription;
struct Context;
struct Viewport;

class AbstractRenderer: public noncopyable {
protected:
	AbstractRenderer(SharedVertexShader, SharedFragmentShader);
	virtual ~AbstractRenderer() = 0;
	const Program m_Program;
	const GLuint gViewport;
	const GLuint gImage;
	const GLuint gPan;
	const GLuint gTextureSampler;
	const SharedMesh m_pMesh;
};

class TextRenderer: public AbstractRenderer {
public:
	TextRenderer(const char *glyphsFilename);
	void draw(const Viewport &viewport, const char* pText);
private:
	Attributes m_Attributes;
	TextureRectangle m_GlyphsTexture;
	const GLuint gChar;
};

} /* namespace duke */

#endif /* TEXTRENDERER_H_ */
