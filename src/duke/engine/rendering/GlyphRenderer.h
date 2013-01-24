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
#include <duke/imageio/Attributes.h>

namespace duke {

struct Viewport;

struct GlyphRenderer : public noncopyable {
    GlyphRenderer(const char *glyphsFilename);
    Binder<TextureRectangle> begin(const Viewport &viewport);
    void setAlpha(float alpha);
    void setZoom(float zoom);
    void setPosition(int x, int y);
    void draw(const char glyph);
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
    TextureRectangle m_GlyphsTexture;
    std::unique_ptr<Binder<TextureRectangle>> m_pTextureBind;
};

void drawText(GlyphRenderer &renderer, const Viewport &viewport, const char* pText, int x, int y, float alpha = 1, float zoom = 1);

} /* namespace duke */
#endif /* GLYPHRENDERER_H_ */
