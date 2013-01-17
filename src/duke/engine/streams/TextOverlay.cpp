/*
 * TextOverlay.cpp
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#include "TextOverlay.h"

#include <duke/engine/renderers/TextRenderer.h>
#include <duke/engine/Context.h>
#include <sstream>

using namespace std;

namespace duke {

TextOverlay::TextOverlay(const std::shared_ptr<TextRenderer>& pTextRenderer, const std::string& string) :
		m_pTextRenderer(pTextRenderer), m_String(string) {
}

void TextOverlay::doRender(const Context& context) const {
	glCheckError();
	m_pTextRenderer->draw(context.viewport, m_String.c_str());
	glCheckError();
}

} /* namespace duke */
