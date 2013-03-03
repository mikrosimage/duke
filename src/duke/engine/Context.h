/*
 * Context.h
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <duke/engine/FitMode.h>
#include <duke/engine/ColorSpace.h>
#include <duke/engine/Viewport.h>
#include <duke/time/FrameUtils.h>
#include <glm/glm.hpp>

struct Attributes;

namespace duke {

struct Texture;
struct PackedFrameDescriptionAndAttributes;
struct GeometryRenderer;
struct GlyphRenderer;

struct Context {
	// drawing
	GeometryRenderer *pGeometryRenderer;
	GlyphRenderer *pGlyphRenderer;
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
	ColorSpace colorSpace = ColorSpace::Auto;
	// file
	std::string filename;
	// current drawing
	const PackedFrameDescriptionAndAttributes *pCurrentImage;
};

} /* namespace duke */
#endif /* CONTEXT_H_ */
