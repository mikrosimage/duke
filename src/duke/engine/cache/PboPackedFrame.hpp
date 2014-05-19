#pragma once

#include <duke/image/ImageDescription.hpp>
#include <duke/gl/GlObjects.hpp>

#include <memory>

namespace duke {

struct PboPackedFrame : public ImageDescription {
  PboPackedFrame() = default;
  PboPackedFrame(const ImageDescription &other) : ImageDescription(other) {}
  std::shared_ptr<gl::GlStreamUploadPbo> pPbo;
};

} /* namespace duke */
