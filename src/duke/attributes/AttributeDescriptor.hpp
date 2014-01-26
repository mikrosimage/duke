#pragma once

#include <duke/base/Check.hpp>
#include <duke/attributes/AttributeEntry.hpp>

#include <string>
#include <typeinfo>

class AttributeDescriptor {
public:
    virtual ~AttributeDescriptor(){}
    virtual std::string dataToString(const AttributeEntry&) const = 0;
    virtual const char* typeToString(const AttributeEntry&) const = 0;
};

template<typename T>
class TypedAttributeDescriptor : public AttributeDescriptor {
    const char* m_pTypename;
public:
    TypedAttributeDescriptor(const char* pTypename) : m_pTypename(pTypename) {
    }
    virtual ~TypedAttributeDescriptor() {}
    virtual std::string dataToString(const AttributeEntry& entry) const override {
        CHECK(sizeof(T) == entry.data.size());
        return std::to_string(*reinterpret_cast<const T*>(entry.data.data()));
    }
    virtual const char* typeToString(const AttributeEntry& entry) const override {
        return m_pTypename ? m_pTypename : entry.pKey;
    }
};

template<>
std::string TypedAttributeDescriptor<const char*>::dataToString(const AttributeEntry& entry) const;

template<>
std::string TypedAttributeDescriptor<std::string>::dataToString(const AttributeEntry& entry) const;
