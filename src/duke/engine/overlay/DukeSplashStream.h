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

namespace duke {

class DukeSplashStream: public duke::IOverlay {
public:
	virtual ~DukeSplashStream();
	virtual void render(const Context&) const;
private:
	GlyphRenderer renderer;
};

} /* namespace duke */
#endif /* DUKESPLASHSTREAM_H_ */
