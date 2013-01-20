/*
 * SingleFrameStream.cpp
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#include "SingleFrameStream.h"
#include <duke/gl/GL.h>
#include <duke/engine/ImageLoadUtils.h>
#include <duke/engine/Context.h>
#include <algorithm>

namespace duke {

static bool attributeLess(const Attribute &a, const Attribute &b) {
	return a.name() < b.name();
}

SingleFrameStream::SingleFrameStream(const char *pFilename) {
	load(pFilename, m_RectangleTexture, m_Attributes, m_Error);
	std::sort(m_Attributes.begin(), m_Attributes.end(), &attributeLess);
}

void SingleFrameStream::doRender(const Context &context) const {
	if (m_Error.empty())
		context.renderTexture(m_RectangleTexture, m_Attributes);
}

const Attributes& SingleFrameStream::getAttributes() const {
	return m_Attributes;
}

} /* namespace duke */
