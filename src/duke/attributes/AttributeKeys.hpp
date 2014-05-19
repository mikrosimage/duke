#pragma once

#include <duke/attributes/PredefinedAttribute.hpp>
#include <cstdint>

namespace attribute {

DECLARE_ATTRIBUTE(Error, const char*, "duke:error", "");
DECLARE_ATTRIBUTE(File, const char*, "duke:filename", "N/A");

DECLARE_ATTRIBUTE(OiioColorspace, const char*, "duke:frame colorspace", "");

DECLARE_ATTRIBUTE(DpxImageOrientation, uint8_t, "duke:frame orientation", 1);
DECLARE_ATTRIBUTE(DpxImageSwapEndianness, bool, "duke:swap endianness", false);
DECLARE_ATTRIBUTE(ImageSwapRedAndBlue, bool, "duke:swap red/blue", false);

DECLARE_ATTRIBUTE(MediaFrameCount, uint64_t, "duke:frame count", 1);

} /* namespace attribute */
