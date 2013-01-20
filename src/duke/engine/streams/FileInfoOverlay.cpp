/*
 * FileInfoOverlay.cpp
 *
 *  Created on: Jan 9, 2013
 *      Author: Guillaume Chatelet
 */

#include "FileInfoOverlay.h"

#include <duke/engine/renderers/TextRenderer.h>
#include <duke/engine/streams/SingleFrameStream.h>
#include <duke/engine/Context.h>

#include <sstream>
#include <iostream>
using namespace std;

namespace duke {
FileInfoOverlay::FileInfoOverlay(const std::shared_ptr<TextRenderer>& pTextRenderer, const std::shared_ptr<SingleFrameStream>& pFileStream) :
		m_pTextRenderer(pTextRenderer), m_pFileStream(pFileStream) {
}

void FileInfoOverlay::doRender(const Context& context) const {
	if (!m_pFileStream)
		return;
	std::ostringstream oss;
	oss << m_pFileStream->getAttributes();
	m_pTextRenderer->draw(context.viewport, oss.str().c_str());
}

} /* namespace duke */
