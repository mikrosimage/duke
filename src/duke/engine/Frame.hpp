#pragma once

#include <duke/io/ImageDescription.hpp>
#include <duke/io/Attributes.hpp>
#include <memory>
#include <string>

namespace duke {

struct Frame {
  FrameDescription description;
  Attributes attributes;
  std::shared_ptr<void> pData;
};

} /* namespace duke */
