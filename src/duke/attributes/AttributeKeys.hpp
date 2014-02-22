#pragma once

#include <duke/attributes/Attribute.hpp>

namespace attribute {

DECLARE_ATTRIBUTE(const char*, Error, nullptr);
DECLARE_ATTRIBUTE(const char*, File, nullptr);

DECLARE_ATTRIBUTE(const char*, OiioColorspace, nullptr);

DECLARE_ATTRIBUTE(uint8_t, DpxImageOrientation, 1);

DECLARE_ATTRIBUTE(uint64_t, MediaFrameCount, 0);
DECLARE_ATTRIBUTE(uint64_t, MediaFrame, 0);

} /* namespace attribute */
