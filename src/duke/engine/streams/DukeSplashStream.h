/*
 * DukeSplashStream.h
 *
 *  Created on: Jan 24, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef DUKESPLASHSTREAM_H_
#define DUKESPLASHSTREAM_H_

#include "MediaStream.h"
#include <duke/engine/rendering/GlyphRenderer.h>

namespace duke {

class DukeSplashStream : public duke::IMediaStream {
public:
    virtual ~DukeSplashStream();
    virtual void doRender(const Context&) const;
private:
    GlyphRenderer renderer;
};

} /* namespace duke */
#endif /* DUKESPLASHSTREAM_H_ */
