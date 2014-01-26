#pragma once

//#include <duke/base/SmallVector.hpp>
//
//typedef SmallVector<char, uint16_t, 128 - sizeof(uint16_t)> AttributeData;

#include <vector>

typedef std::vector<char> AttributeData;

class AttributeDescriptor;

struct AttributeEntry {
    const char* pKey;
    const AttributeDescriptor* pDescriptor;
    AttributeData data;
    AttributeEntry() : AttributeEntry(nullptr, nullptr, { }) { }
    AttributeEntry(const char* pKey, const AttributeDescriptor* pDescriptor, AttributeData data) :
                    pKey(pKey), pDescriptor(pDescriptor), data(data) { }
};
