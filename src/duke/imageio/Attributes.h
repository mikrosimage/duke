/*
 * Attributes.h
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef ATTRIBUTES_H_
#define ATTRIBUTES_H_

#include "Attribute.h"
#include <stdexcept>

struct Attributes: public std::vector<Attribute> {
	template<typename T>
	const Attribute* find(const char* name) const {
		return find(name, ptraits<T>::value,false);
	}
	template<typename T>
	Attribute::TypedVectorAttribute<T> findVector(const char* name) const {
		const Attribute* pAttr = find(name, ptraits<T>::value,true);
		if (pAttr == nullptr)
			throw std::runtime_error("vector not found");
		return Attribute::TypedVectorAttribute<T>(pAttr);
	}

	const Attribute* find(const char* name, PrimitiveType type, bool isVector=false) const {
		for (const Attribute &attr : *this)
			if (attr.type() == type && attr.name() == name && attr.isVector() == isVector)
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

#endif /* ATTRIBUTES_H_ */
