#pragma once

#include <duke/engine/FitMode.hpp>
#include <duke/engine/ColorSpace.hpp>
#include <duke/engine/Viewport.hpp>
#include <duke/time/FrameUtils.hpp>
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
	FitMode fitMode = FitMode::FREE;
	float zoom = 1;
	glm::ivec2 pan;
	// grading
	glm::bvec4 channels = glm::bvec4(false);
	float exposure = 1;
	float gamma = 1;
	ColorSpace fileColorSpace = ColorSpace::Auto;
	ColorSpace screenColorSpace = ColorSpace::Auto;
	// file
	std::string filename;
	// current drawing
	const PackedFrameDescriptionAndAttributes *pCurrentImage;
};

} /* namespace duke */
