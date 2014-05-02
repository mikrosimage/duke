#pragma once

#include <duke/imageio/FrameDescriptionAndAttributes.hpp>
#include <duke/gl/GlObjects.hpp>
#include <memory>

namespace duke {

struct PboPackedFrame : public FrameDescriptionAndAttributes {
  PboPackedFrame() = default;
  PboPackedFrame(const FrameDescriptionAndAttributes &other) : FrameDescriptionAndAttributes(other) {}
  std::shared_ptr<gl::GlStreamUploadPbo> pPbo;
};

} /* namespace duke */
