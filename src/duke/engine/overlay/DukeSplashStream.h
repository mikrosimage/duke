/*
 * DukeSplashStream.h
 *
 *  Created on: Jan 24, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef DUKESPLASHSTREAM_H_
#define DUKESPLASHSTREAM_H_

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
	GlyphRenderer renderer;
	AnimationData m_RightAlpha;
	AnimationData m_LeftAlpha;
	AnimationData m_LeftPos;
};

} /* namespace duke */
#endif /* DUKESPLASHSTREAM_H_ */
