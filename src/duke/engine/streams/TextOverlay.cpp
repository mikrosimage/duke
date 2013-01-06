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
	ostringstream oss;
	oss << m_String << " " << context.clipFrame << " " << context.exposure;
	checkError();
	m_pTextRenderer->draw(context.viewport, oss.str().c_str());
	checkError();
}

} /* namespace duke */
