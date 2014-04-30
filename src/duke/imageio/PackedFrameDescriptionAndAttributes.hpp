#pragma once

#include <duke/imageio/PackedFrameDescription.hpp>
#include <duke/attributes/Attributes.hpp>

namespace duke {

struct PackedFrameDescriptionAndAttributes {
  PackedFrameDescription description;
  attribute::Attributes attributes;
};

} /* namespace duke */
