/*
 * DukeSplashStream.h
 *
 *  Created on: Jan 24, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include "IOverlay.h"
#include <duke/engine/rendering/GlyphRenderer.h>
#include <duke/animation/Animation.h>

namespace duke {

class DukeSplashStream: public duke::IOverlay {
public:
	DukeSplashStream();
	virtual ~DukeSplashStream();
	virtual void render(const Context&) const;
private:
	AnimationData m_RightAlpha;
	AnimationData m_LeftAlpha;
	AnimationData m_LeftPos;
};

} /* namespace duke */
