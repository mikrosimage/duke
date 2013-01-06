/*
 * TextRenderer.h
 *
 *  Created on: Jan 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include "ImageRenderer.h"
#include <duke/engine/LoadableTexture.h>

namespace duke {

class TextRenderer: public AbstractRenderer {
public:
	TextRenderer(const char *glyphsFilename);
	void draw(const duke::Viewport &viewport, const char* pText);
private:
	LoadableTexture m_GlyphsTexture = (GL_TEXTURE_RECTANGLE);
	const GLuint gChar;
};

} /* namespace duke */

#endif /* TEXTRENDERER_H_ */
