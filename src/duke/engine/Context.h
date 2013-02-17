/*
 * Context.h
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "DukeWindow.h"
#include <duke/engine/ColorSpace.h>
#include <duke/time/FrameUtils.h>
#include <glm/glm.hpp>

struct Attributes;

namespace duke {

class Texture;
struct PackedFrameDescriptionAndAttributes;

enum class FitMode {
	ACTUAL, FREE, INNER, OUTER
};

struct Context {
	// timing
	Time liveTime;
	Time playbackTime;
	FrameIndex currentFrame;
	size_t clipFrame = 0;
	// geometry
	Viewport viewport;
	FitMode fitMode = FitMode::ACTUAL;
	float zoom = 1;
	glm::ivec2 pan;
	// grading
	glm::bvec4 channels = glm::bvec4(false);
	float exposure = 1;
	float gamma = 1;
	ColorSpace colorSpace = ColorSpace::Source;
	// file
	std::string filename;
	// current drawing
	const PackedFrameDescriptionAndAttributes *pCurrentImage;
};

} /* namespace duke */
#endif /* CONTEXT_H_ */
