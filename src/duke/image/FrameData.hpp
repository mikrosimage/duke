#pragma once

#include <duke/image/FrameDescriptionAndAttributes.hpp>

#include <memory>

namespace duke {

struct FrameData : public FrameDescriptionAndAttributes {
  std::shared_ptr<char> pData;
};

} /* namespace duke */
