/*
 * AttributesOverlay.cpp
 *
 *  Created on: Feb 16, 2013
 *      Author: Guillaume Chatelet
 */

#include "AttributesOverlay.h"
#include <duke/engine/Context.h>
#include <duke/engine/rendering/GlyphRenderer.h>
#include <sstream>

namespace duke {

AttributesOverlay::AttributesOverlay(const std::shared_ptr<GlyphRenderer> &pGlyphRenderer) :
		m_pGlyphRenderer(pGlyphRenderer) {
}

void AttributesOverlay::render(const Context &context) const {
	if (!context.pCurrentAttributes)
		return;
	std::ostringstream oss;
	oss << *context.pCurrentAttributes;
	drawText(*m_pGlyphRenderer, context.viewport, oss.str().c_str(), 100, 100, 1, 2);
}

} /* namespace duke */
