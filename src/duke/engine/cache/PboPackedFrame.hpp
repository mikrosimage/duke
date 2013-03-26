#pragma once

#include <duke/imageio/PackedFrameDescriptionAndAttributes.hpp>
#include <duke/gl/GlObjects.hpp>
#include <memory>

namespace duke {

struct PboPackedFrame: public PackedFrameDescriptionAndAttributes {
	PboPackedFrame() = default;
	PboPackedFrame(const PackedFrameDescriptionAndAttributes &other) :
	PackedFrameDescriptionAndAttributes(other) {
	}
	std::shared_ptr<gl::GlStreamUploadPbo> pPbo;
};

} /* namespace duke */
