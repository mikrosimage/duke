#pragma once

#include <duke/attributes/Attributes.hpp>
#include <cstdint>

namespace attribute {

RegisterStringAttribute(Error, nullptr);
RegisterStringAttribute(File, nullptr);

RegisterStringAttribute(OiioColorspace, nullptr);

RegisterValueAttribute(uint8_t, DpxImageOrientation, 1);

RegisterValueAttribute(uint64_t, MediaFrameCount, 0LL);
RegisterValueAttribute(uint64_t, MediaFrame, 0LL);

} /* namespace attribute */
