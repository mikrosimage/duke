/*
 * TextOverlay.cpp
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#include "TextOverlay.h"
#include <duke/engine/Context.h>

namespace duke {

TextOverlay::TextOverlay(TextRenderer& textRenderer, const std::string& string) :
		m_TextRenderer(textRenderer), m_String(string) {
}

void TextOverlay::doRender(const Context& context) const {
	m_TextRenderer.draw(context.viewport, m_String.c_str());
}

} /* namespace duke */
