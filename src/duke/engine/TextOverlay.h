/*
 * TextOverlay.h
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTOVERLAY_H_
#define TEXTOVERLAY_H_

#include "MediaStream.h"
#include <duke/engine/renderers/TextRenderer.h>
#include <string>

namespace duke {

class TextOverlay: public MediaStream {
public:
	TextOverlay(TextRenderer& textRenderer, const std::string &string);
	virtual void doRender(const Context&) const;
private:
	TextRenderer &m_TextRenderer;
	std::string m_String;
};

} /* namespace duke */

#endif /* TEXTOVERLAY_H_ */
