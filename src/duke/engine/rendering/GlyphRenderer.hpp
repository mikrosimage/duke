#pragma once

#include <duke/base/NonCopyable.hpp>
#include <duke/gl/Textures.hpp>
#include <duke/gl/Program.hpp>
#include <duke/gl/Mesh.hpp>
#include <duke/attributes/Attributes.hpp>

namespace duke {

struct Viewport;
struct GeometryRenderer;

struct GlyphRenderer: public noncopyable {
	typedef gl::Binder<gl::GlTextureObject> GlyphBinder;
	GlyphRenderer(const GeometryRenderer &renderer, const char *glyphsFilename = ".duke_ascii_font");
	GlyphBinder begin(const Viewport &viewport) const;
	void setAlpha(float alpha) const;
	void setZoom(float zoom) const;
	void draw(int x, int y, const char glyph) const;

	const GeometryRenderer &getGeometryRenderer() const;

private:
	const GeometryRenderer &m_GeometryRenderer;
	mutable Program m_Program;
	Attributes m_Attributes;
	Texture m_GlyphsTexture;

	GlyphBinder m_pTextureBind;
};

void drawText(const GlyphRenderer &renderer, const Viewport &viewport, const char* pText, int x, int y, float alpha = 1, float zoom = 1);

} /* namespace duke */
