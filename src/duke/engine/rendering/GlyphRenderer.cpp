/*
 * GlyphRenderer.cpp
 *
 *  Created on: Jan 24, 2013
 *      Author: Guillaume Chatelet
 */

#include "GlyphRenderer.h"
#include <duke/engine/DukeWindow.h>
#include <duke/engine/ImageLoadUtils.h>

namespace duke {

static const char * const pTextVertexShader =
		R"(
#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;

uniform ivec2 gViewport;
uniform ivec2 gImage;
uniform ivec2 gPan;
uniform float gZoom;
uniform int gChar;

out vec2 vVaryingTexCoord; 

mat4 ortho(int left, int right, int bottom, int top) {
    mat4 Result = mat4(1);
    Result[0][0] = float(2) / (right - left);
    Result[1][1] = float(2) / (top - bottom);
    Result[2][2] = - float(1);
    Result[3][3] = 1;
    Result[3][0] = - (right + left) / (right - left);
    Result[3][1] = - (top + bottom) / (top - bottom);
    return Result;
}

mat4 translate(mat4 m, vec3 v) {
    mat4 Result = mat4(m);
    Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
    return Result;
}

mat4 scale(mat4 m, vec3 v) {
    mat4 Result = mat4(m);
    Result[0] = m[0] * v[0];
    Result[1] = m[1] * v[1];
    Result[2] = m[2] * v[2];
    Result[3] = m[3];
    return Result;
}

void main() {
    const ivec2 tiles = ivec2(16);
    ivec2 translating = ivec2(0); // translation must be integer to prevent aliasing
    translating /= 2; // moving to center
    translating += gPan; // moving to center
    vec2 scaling = vec2(1);
    scaling /= 2; // bringing square from [-1,1] to [-.5,.5]
    scaling /= tiles; // to pixel dimension
    scaling *= gImage; // to pixel dimension
    scaling *= gZoom; // zooming
    mat4 world = mat4(1);
    world = translate(world, vec3(translating, 0)); // move to center
    world = scale(world, vec3(scaling, 1));
    mat4 proj = ortho(0, gViewport.x, 0, gViewport.y);
    mat4 worldViewProj = proj * world;
    gl_Position = worldViewProj * vec4(Position, 1.0);
    ivec2 charDim = abs(gImage) / tiles;
    ivec2 charPos = ivec2(gChar%tiles.x, gChar/tiles.y);
    vVaryingTexCoord = charDim*(UV+charPos);
})";

static const char * const pTextFragmentShader =
		R"(#version 330

out vec4 vFragColor;
uniform sampler2DRect rectangleImageSampler;
uniform float gAlpha;
smooth in vec2 vVaryingTexCoord;

void main(void)
{
    vec4 sample = texture(rectangleImageSampler, vVaryingTexCoord);
    sample.a *= gAlpha;
    vFragColor = sample;
})";

GlyphRenderer::GlyphRenderer(const char *glyphsFilename) :
		m_pMesh(getSquare()), //
		m_Program(makeVertexShader(pTextVertexShader), makeFragmentShader(pTextFragmentShader)), //
		gTextureSampler(m_Program.getUniformLocation("rectangleImageSampler")), //
		gViewport(m_Program.getUniformLocation("gViewport")), //
		gImage(m_Program.getUniformLocation("gImage")), //
		gPan(m_Program.getUniformLocation("gPan")), //
		gChar(m_Program.getUniformLocation("gChar")), //
		gZoom(m_Program.getUniformLocation("gZoom")), //
		gAlpha(m_Program.getUniformLocation("gAlpha")) //
{
	std::string error;
	if (!load(glyphsFilename, m_GlyphsTexture, m_Attributes, error))
		throw std::runtime_error("unable to load glyphs texture");
}

GlyphRenderer::GlyphBinder GlyphRenderer::begin(const Viewport &viewport) const {
	m_Program.use();
	glUniform2i(gViewport, viewport.dimension.x, viewport.dimension.y);
	glUniform1i(gTextureSampler, 0);
	auto scopeBinded = m_GlyphsTexture.scope_bind_texture();
	glTexParameteri(m_GlyphsTexture.target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(m_GlyphsTexture.target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	setTextureDimensions(gImage, m_GlyphsTexture.description.width, m_GlyphsTexture.description.height, m_Attributes.getOrientation());
	return std::move(scopeBinded);
}

void GlyphRenderer::setAlpha(float alpha) const {
	glUniform1f(gAlpha, alpha);
}

void GlyphRenderer::setZoom(float zoom) const {
	glUniform1f(gZoom, zoom);
}

void GlyphRenderer::setPosition(int x, int y) const {
	glUniform2i(gPan, x, y);
}

void GlyphRenderer::draw(const char glyph) const {
	glUniform1i(gChar, glyph);
	m_pMesh->draw();
}

void drawText(const GlyphRenderer &renderer, const Viewport &viewport, const char* pText, int x, int y, float alpha, float zoom) {
	if (pText == nullptr || *pText == '\0')
		return;
	const int xOrigin = x;
	const auto bound = renderer.begin(viewport);
	renderer.setAlpha(alpha);
	renderer.setZoom(zoom);
	for (; *pText != '\0'; ++pText) {
		const char c = *pText;
		if (c == '\n') {
			x = xOrigin;
			y += 8 * zoom;
			continue;
		}
		renderer.setPosition(x, y);
		renderer.draw(c);
		x += 8 * zoom;
	}
}

} /* namespace duke */
