#pragma once

#include <duke/attributes/Attribute.hpp>

namespace attribute {

DECLARE_ATTRIBUTE(const char*, DukeError, nullptr);
DECLARE_ATTRIBUTE(const char*, DukeFilePathKey, nullptr);

DECLARE_ATTRIBUTE(const char*, OiioColorspace, nullptr);

DECLARE_ATTRIBUTE(uint8_t, DpxImageOrientation, 1);

} /* namespace attribute */
