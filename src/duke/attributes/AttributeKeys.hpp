#pragma once

#include <duke/attributes/PredefinedAttribute.hpp>
#include <cstdint>

namespace attribute {

DECLARE_ATTRIBUTE(Error, const char*, "dk:error", "");
DECLARE_ATTRIBUTE(File, const char*, "dk:filename", "");

DECLARE_ATTRIBUTE(OiioColorspace, const char*, "dk:frame colorspace", "");

DECLARE_ATTRIBUTE(DpxImageOrientation, uint8_t, "dk:frame orientation", 1);

DECLARE_ATTRIBUTE(MediaFrameCount, uint64_t, "dk:frame count", 1);
DECLARE_ATTRIBUTE(MediaFrame, uint64_t, "dk:frame number", 0);

} /* namespace attribute */
