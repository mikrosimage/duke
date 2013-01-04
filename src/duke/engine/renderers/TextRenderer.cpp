/*
 * TextRenderer.cpp
 *
 *  Created on: Jan 3, 2013
 *      Author: Guillaume Chatelet
 */

#include "TextRenderer.h"

#include <GL/gl.h>

#include <stdexcept>

TextRenderer::TextRenderer(const char *glyphsFilename) :
		m_TextProgram(loadVertexShader("shader/vertex_text.glsl"), loadFragmentShader("shader/basic.fglsl")), //
		gViewport(m_TextProgram.getUniformLocation("gViewport")), //
		gImage(m_TextProgram.getUniformLocation("gImage")), //
		gPan(m_TextProgram.getUniformLocation("gPan")), //
		gChar(m_TextProgram.getUniformLocation("gChar")), //
		gTextureSampler(m_TextProgram.getUniformLocation("rectangleImageSampler")), //
		m_pMesh(getSquare()) {
	if (!m_GlyphsTexture.load(glyphsFilename, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE))
		throw std::runtime_error("unable to load glyphs texture");
}

void TextRenderer::draw(const duke::Viewport &viewport, const char* pText) {
	m_TextProgram.use();
	glUniform2i(gViewport, viewport.dimension.x, viewport.dimension.y);
	glUniform1i(gTextureSampler, 0);
	const auto scopeBind = m_GlyphsTexture.use(gImage);
	const auto tileWidth = m_GlyphsTexture.description.width / 16;
	const auto tileHeight = m_GlyphsTexture.description.height / 16;
	size_t x = tileWidth * 2;
	for (; *pText != 0; ++pText, x += tileWidth) {
		glUniform1i(gChar, *reinterpret_cast<const unsigned char*>(pText));
		glUniform2i(gPan, x, tileHeight * 2);
		m_pMesh->draw();
	}
}
