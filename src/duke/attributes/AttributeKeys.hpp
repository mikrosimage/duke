#pragma once

#include <string>

namespace attribute {

struct AttributeKey {
    virtual ~AttributeKey() {}
    virtual const char* name() const = 0;
};

#define DECLARE_ATTRIBUTE(TYPE, NAME, DEFAULT) \
struct NAME : public AttributeKey { \
    typedef TYPE value_type; \
    inline static TYPE default_value() {return DEFAULT; } \
    virtual const char* name() const override { return #NAME; } \
}

DECLARE_ATTRIBUTE(const char*, DukeError, nullptr);
DECLARE_ATTRIBUTE(const char*, DukeFilePathKey, nullptr);
DECLARE_ATTRIBUTE(uint8_t, DpxImageOrientation, 1);
DECLARE_ATTRIBUTE(const char*, OiioColorspace, nullptr);

} /* namespace attribute */
