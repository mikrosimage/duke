/*
 * TextOverlay.h
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef TEXTOVERLAY_H_
#define TEXTOVERLAY_H_

#include "MediaStream.h"
#include <string>
#include <memory>

namespace duke {

class TextRenderer;

class TextOverlay: public MediaStream {
public:
	TextOverlay(const std::shared_ptr<TextRenderer>& pTextRenderer, const std::string &string);
	virtual void doRender(const Context&) const;
private:
	const std::shared_ptr<TextRenderer> m_pTextRenderer;
	std::string m_String;
};

} /* namespace duke */

#endif /* TEXTOVERLAY_H_ */
