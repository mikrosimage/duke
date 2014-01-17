#pragma once

#include <string>

namespace attribute {

struct AttributeKey {
    virtual ~AttributeKey() {}
    virtual const char* name() const = 0;
};

#define DECLARE_ATTRIBUTE(TYPE, NAME) \
struct NAME : public AttributeKey { \
    typedef TYPE value_type; \
    virtual const char* name() const override { return #NAME; } \
}

DECLARE_ATTRIBUTE(const char*, DukeError);
DECLARE_ATTRIBUTE(const char*, DukeFilePathKey);
DECLARE_ATTRIBUTE(uint8_t, DpxImageOrientation);
DECLARE_ATTRIBUTE(const char*, OiioColorspace);

} /* namespace attribute */
