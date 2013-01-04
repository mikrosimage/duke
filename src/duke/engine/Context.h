/*
 * Context.h
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "DukeWindow.h"
#include <duke/time/FrameUtils.h>

namespace duke {

struct Context {
	Viewport viewport;
	Time liveTime;
	Time playbackTime;
	Frame currentFrame;
};

} /* namespace duke */
#endif /* CONTEXT_H_ */
