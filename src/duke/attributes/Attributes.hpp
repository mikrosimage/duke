#pragma once

#include "Attribute.hpp"

#include <duke/base/StringUtils.hpp>

#include <stdexcept>

struct Attributes: public std::vector<Attribute> {
	template<typename T>
	const Attribute* find(const char* name) const {
		return find(name, ptraits<T>::value, false);
	}
	template<typename T>
	Attribute::TypedVectorAttribute<T> findVector(const char* name) const {
		const Attribute* pAttr = find(name, ptraits<T>::value, true);
		if (pAttr == nullptr)
			throw std::runtime_error("vector not found");
		return Attribute::TypedVectorAttribute<T>(pAttr);
	}
	const char* findString(const char* name) const {
		const auto pFound = find(name, ptraits<char>::value, true);
		if (pFound)
			return pFound->getString();
		return nullptr;
	}
	int getOrientation() const {
		const auto pOrientation = find<int>("Orientation");
		if (pOrientation)
			return pOrientation->getScalar<int>();
		return 1;
	}
	const Attribute* find(const char* name, PrimitiveType type, bool isVector = false) const {
		for (const Attribute &attr : *this)
			if (streq(attr.name(), name) && attr.type() == type && attr.isVector() == isVector)
				return &attr;
		return nullptr;
	}
	friend std::ostream& operator<<(std::ostream& stream, const Attributes&attributes) {
		bool first = true;
		for (const auto &attribute : attributes) {
			if (!first)
				stream << std::endl;
			stream << attribute;
			first = false;
		}
		return stream;
	}
};
