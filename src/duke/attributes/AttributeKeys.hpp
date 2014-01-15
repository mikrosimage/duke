#pragma once

#include <string>

namespace attribute {

extern const char pDukeError[];
extern const char pDukeFilePathKey[];
extern const char pOiioColospaceKey[];
extern const char pOiioGammaKey[];
extern const char pOrientationKey[];

struct AttributeKey {
    virtual ~AttributeKey() {}
    virtual const char* name() const = 0;
};

#define DECLARE_ATTRIBUTE(TYPE, NAME) \
struct NAME : public AttributeKey { \
    typedef TYPE value_type; \
    virtual const char* name() const override { return #NAME; } \
}

DECLARE_ATTRIBUTE(std::string, DukeError);
DECLARE_ATTRIBUTE(std::string, DukeFilePathKey);

} /* namespace attribute */
