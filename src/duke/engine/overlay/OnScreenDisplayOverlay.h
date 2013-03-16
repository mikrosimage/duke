/*
 * StatusOverlay.h
 *
 *  Created on: Feb 19, 2013
 *      Author: Guillaume Chatelet
 */
#pragma once

#include "IOverlay.h"
#include <duke/time/FrameUtils.h>
#include <duke/animation/Animation.h>
#include <string>
#include <memory>

namespace duke {

struct Context;
struct GlyphRenderer;

class OnScreenDisplayOverlay: public duke::IOverlay {
public:
	OnScreenDisplayOverlay(const GlyphRenderer&);

	void setString(const Time& time, const std::string&msg);

	virtual void render(const Context&) const;
private:
	const GlyphRenderer &m_GlyphRenderer;
	Time m_ShowTime;
	std::string m_Message;
	AnimationData m_Alpha;
};

} /* namespace duke */
