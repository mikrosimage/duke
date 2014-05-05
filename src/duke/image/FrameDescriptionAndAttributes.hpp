#pragma once

#include <duke/attributes/Attributes.hpp>
#include <duke/image/FrameDescription.hpp>

namespace duke {

struct FrameDescriptionAndAttributes {
  FrameDescription description;
  attribute::Attributes attributes;
};

} /* namespace duke */
