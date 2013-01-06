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
#include <glm/glm.hpp>

namespace duke {

struct Context {
	Viewport viewport;
	Time liveTime;
	Time playbackTime;
	Frame currentFrame;
	size_t clipFrame = 0;
	int zoom = 1;
	glm::ivec2 pan;
	glm::bvec4 channels = glm::bvec4(false);
	float exposure = 1;
};

} /* namespace duke */
#endif /* CONTEXT_H_ */
