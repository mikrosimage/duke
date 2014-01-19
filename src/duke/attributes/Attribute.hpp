#pragma once

#include <duke/attributes/AttributeDescriptor.hpp>

#include <string>

namespace attribute {

struct AttributeKey {
    virtual ~AttributeKey() {}
    virtual const char* name() const = 0;
    virtual const AttributeDescriptor* descriptor() const = 0;
};

#define DECLARE_ATTRIBUTE(TYPE, NAME, DEFAULT) \
struct NAME : public AttributeKey { \
    typedef TYPE value_type; \
    inline static TYPE default_value() {return DEFAULT; } \
    virtual const char* name() const override { return #NAME; } \
    virtual const AttributeDescriptor* descriptor() const override { \
        const static TypedAttributeDescriptor<TYPE> descriptor(#TYPE); \
        return &descriptor; \
    } \
}

} /* namespace attribute */
