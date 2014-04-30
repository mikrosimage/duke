#pragma once

#include <duke/imageio/PackedFrameDescriptionAndAttributes.hpp>
#include <memory>

namespace duke {

struct RawPackedFrame : public PackedFrameDescriptionAndAttributes {
  std::shared_ptr<char> pData;
};

} /* namespace duke */
