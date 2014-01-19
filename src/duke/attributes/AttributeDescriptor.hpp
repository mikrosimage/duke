#pragma once

#include <duke/base/Check.hpp>

#include <string>
#include <typeinfo>

class AttributeDescriptor {
public:
    virtual ~AttributeDescriptor(){}
    virtual std::string dataToString(const void* pData, size_t size) const = 0;
    virtual const char* typeToString() const = 0;
};

template<typename T>
class TypedAttributeDescriptor : public AttributeDescriptor {
    const char* m_pTypename;
public:
    TypedAttributeDescriptor(const char* pTypename) : m_pTypename(pTypename) {
        CHECK(m_pTypename);
    }
    virtual ~TypedAttributeDescriptor() {}
    virtual std::string dataToString(const void* pData, size_t size) const override {
        CHECK(sizeof(T) == size);
        return std::to_string(*reinterpret_cast<const T*>(pData));
    }
    virtual const char* typeToString() const override {
        return m_pTypename;
    }
};

template<>
std::string TypedAttributeDescriptor<const char*>::dataToString(const void* pData, size_t size) const;

template<>
std::string TypedAttributeDescriptor<std::string>::dataToString(const void* pData, size_t size) const;
