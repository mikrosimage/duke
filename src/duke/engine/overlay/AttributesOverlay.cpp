#include "AttributesOverlay.hpp"
#include <duke/engine/Context.hpp>
#include <duke/engine/rendering/GlyphRenderer.hpp>
#include <duke/imageio/PackedFrameDescriptionAndAttributes.hpp>
#include <sstream>
#include <algorithm>

namespace duke {

AttributesOverlay::AttributesOverlay(const GlyphRenderer &glyphRenderer) :
		m_GlyphRenderer(glyphRenderer) {
}

void AttributesOverlay::render(const Context &context) const {
	if (!context.pCurrentImage)
		return;
	std::ostringstream oss;
	oss << context.pCurrentImage->attributes;
	drawText(m_GlyphRenderer, context.viewport, oss.str().c_str(), 50, 50, 1, 2, context.isPlaying);
}

} /* namespace duke */
