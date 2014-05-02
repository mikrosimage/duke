#pragma once

#include <duke/imageio/FrameDescription.hpp>
#include <duke/attributes/Attributes.hpp>

namespace duke {

struct FrameDescriptionAndAttributes {
  FrameDescription description;
  attribute::Attributes attributes;
};

} /* namespace duke */
