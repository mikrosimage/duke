#pragma once

class AttributeDescriptor {
public:
    virtual ~AttributeDescriptor(){}
    virtual void appendDataToString(const void* pData, size_t size, std::string& output) const = 0;
};
