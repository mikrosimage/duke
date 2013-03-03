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

StatusOverlay::StatusOverlay(const GlyphRenderer &glyphRenderer) :
		m_GlyphRenderer(glyphRenderer) {
	m_Alpha.duration = 800;
	m_Alpha.type = EasingCurve::InExpo;
}

void StatusOverlay::setString(const Time& time, const std::string &msg) {
	m_ShowTime = time;
	m_Message = msg;
}

void StatusOverlay::render(const Context& context) const {
	const Time time = context.liveTime - m_ShowTime;
	const auto ms = time.asMilliseconds();
	const double alpha = interpolateValue<double>(m_Alpha, 1, 0, ms);
	if (alpha > 0)
		drawText(m_GlyphRenderer, //
				context.viewport, //
				m_Message.c_str(), //
				100, 100, //
				alpha, //
				3);
}

} /* namespace duke */
