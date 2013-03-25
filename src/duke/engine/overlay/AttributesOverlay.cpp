/*
 * AttributesOverlay.cpp
 *
 *  Created on: Feb 16, 2013
 *      Author: Guillaume Chatelet
 */

#include "AttributesOverlay.h"
#include <duke/engine/Context.h>
#include <duke/engine/rendering/GlyphRenderer.h>
#include <duke/imageio/PackedFrameDescriptionAndAttributes.h>
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
	drawText(m_GlyphRenderer, context.viewport, oss.str().c_str(), 50, 50, 1, 2);
}

} /* namespace duke */
