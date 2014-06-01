#pragma once

#include "duke/attributes/Attribute.hpp"
#include "duke/base/StringAppender.hpp"

namespace attribute {

// Printf from a StringAppender directly
void append(const Attribute& attribute, StringAppender& appender);

}  // namespace attribute
