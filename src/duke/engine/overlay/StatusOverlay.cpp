/*
 * StatusOverlay.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: Guillaume Chatelet
 */

#include "StatusOverlay.h"
#include <duke/engine/rendering/GlyphRenderer.h>
#include <duke/engine/Context.h>
#include <duke/animation/Animation.h>
#include <algorithm>

namespace duke {

StatusOverlay::StatusOverlay(const std::shared_ptr<GlyphRenderer>&pGlyphRenderer) :
		m_pGlyphRenderer(pGlyphRenderer) {
}

void StatusOverlay::setString(const Time& time, const std::string &msg) {
	m_ShowTime = time;
	m_Message = msg;
}

void StatusOverlay::render(const Context& context) const {
	const Time time = context.liveTime - m_ShowTime;
	const auto ms = time.asMilliseconds();
	Animation<float> alpha(800, 1, 0);
	drawText(*m_pGlyphRenderer, //
			context.viewport, //
			m_Message.c_str(), //
			100, 100, //
			alpha.getAnimatedValue(ms, EasingCurveTimeInterpolator(EasingCurve::InExpo)), //
			3);
}

} /* namespace duke */
