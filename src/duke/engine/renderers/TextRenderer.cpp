/*
 * TextRenderer.cpp
 *
 *  Created on: Jan 3, 2013
 *      Author: Guillaume Chatelet
 */

#include "TextRenderer.h"

#include <GL/gl.h>

#include <stdexcept>

namespace duke {

TextRenderer::TextRenderer(const char *glyphsFilename) :
		AbstractRenderer(loadVertexShader("shader/vertex_text.glsl"), loadFragmentShader("shader/basic.fglsl")), //
		gChar(m_Program.getUniformLocation("gChar")) {
	if (!m_GlyphsTexture.load(glyphsFilename, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE))
		throw std::runtime_error("unable to load glyphs texture");
}

void TextRenderer::draw(const duke::Viewport &viewport, const char* pText) {
	m_Program.use();
	glUniform2i(gViewport, viewport.dimension.x, viewport.dimension.y);
	glUniform1i(gTextureSampler, 0);
	const auto scopeBind = m_GlyphsTexture.use(gImage);
	const auto tileWidth = m_GlyphsTexture.description.width / 16;
	const auto tileHeight = m_GlyphsTexture.description.height / 16;
	const size_t x_orig = tileWidth * 2;
	size_t x = x_orig;
	size_t y = viewport.dimension.y - tileHeight * 2;
	for (; *pText != 0; ++pText) {
		if (*pText == '\n') {
			x = x_orig;
			y -= tileHeight + 2;
			continue;
		}
		glUniform1i(gChar, *reinterpret_cast<const unsigned char*>(pText));
		glUniform2i(gPan, x, y);
		m_pMesh->draw();
		x += tileWidth - 2;
	}
}

} /* namespace duke */
