/*
 * GlyphRenderer.h
 *
 *  Created on: Jan 24, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef GLYPHRENDERER_H_
#define GLYPHRENDERER_H_

#include <duke/NonCopyable.h>
#include <duke/gl/Textures.h>
#include <duke/gl/Shader.hpp>
#include <duke/gl/Mesh.hpp>
#include <duke/attributes/Attributes.h>

namespace duke {

struct Viewport;

struct GlyphRenderer : public noncopyable {
	typedef gl::Binder<gl::GlTextureObject> GlyphBinder;
    GlyphRenderer(const char *glyphsFilename = ".duke_ascii_font");
    GlyphBinder begin(const Viewport &viewport) const;
    void setAlpha(float alpha) const;
    void setZoom(float zoom) const;
    void setPosition(int x, int y) const;
    void draw(const char glyph) const;
private:
    const SharedMesh m_pMesh;
    const Program m_Program;
    const GLuint gTextureSampler;
    const GLuint gViewport;
    const GLuint gImage;
    const GLuint gPan;
    const GLuint gChar;
    const GLuint gZoom;
    const GLuint gAlpha;
    Attributes m_Attributes;
    Texture m_GlyphsTexture;

    GlyphBinder m_pTextureBind;
};

void drawText(const GlyphRenderer &renderer, const Viewport &viewport, const char* pText, int x, int y, float alpha = 1, float zoom = 1);

} /* namespace duke */
#endif /* GLYPHRENDERER_H_ */
