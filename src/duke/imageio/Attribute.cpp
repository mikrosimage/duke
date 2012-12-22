/*
 * Attribute.cpp
 *
 *  Created on: Dec 22, 2012
 *      Author: Guillaume Chatelet
 */

#include "Attribute.h"

#define ISTYPE(TYPE) typeIdName<TYPE>()==typeidname

#define TYPENAME(TYPE) if(ISTYPE(TYPE)) return #TYPE;\
		if(ISTYPE(const TYPE*)) return "const "#TYPE"[]"

#define SIGNED_UNSIGNED_TYPENAME(TYPE) TYPENAME(TYPE);\
		TYPENAME(unsigned TYPE)

static const char* getTypeName(const char*typeidname) {
	SIGNED_UNSIGNED_TYPENAME(char);
	SIGNED_UNSIGNED_TYPENAME(short);
	SIGNED_UNSIGNED_TYPENAME(int);
	SIGNED_UNSIGNED_TYPENAME(long);
	SIGNED_UNSIGNED_TYPENAME(long long);
	TYPENAME(float);
	TYPENAME(double);
	throw std::runtime_error("not yet implemented");
}

#define TYPESIZE(TYPE) if(ISTYPE(const TYPE*)) return sizeof(TYPE);
#define SIGNED_UNSIGNED_TYPESIZE(TYPE) if(ISTYPE(const TYPE*)||ISTYPE(const unsigned TYPE*)) return sizeof(TYPE);

static size_t getTypeSize(const char*typeidname) {
	SIGNED_UNSIGNED_TYPESIZE(char);
	SIGNED_UNSIGNED_TYPESIZE(short);
	SIGNED_UNSIGNED_TYPESIZE(int);
	SIGNED_UNSIGNED_TYPESIZE(long);
	SIGNED_UNSIGNED_TYPESIZE(long long);
	TYPESIZE(float);
	TYPESIZE(double);
	throw std::runtime_error("not yet implemented");
}

#define STREAMTYPE(TYPE) if(ISTYPE(TYPE)||ISTYPE(const TYPE*)) { stream << *reinterpret_cast<const TYPE*>(pData); return; }
#define SIGNED_UNSIGNED_STREAMTYPE(TYPE) STREAMTYPE(TYPE)\
STREAMTYPE(unsigned TYPE)

static void displayValue(std::ostream &stream, const void* pData, const char*typeidname) {
	SIGNED_UNSIGNED_STREAMTYPE(int);
	STREAMTYPE(float);
	STREAMTYPE(double);
	SIGNED_UNSIGNED_STREAMTYPE(long);
	SIGNED_UNSIGNED_STREAMTYPE(char);
	SIGNED_UNSIGNED_STREAMTYPE(short);
	SIGNED_UNSIGNED_STREAMTYPE(long long);
	throw std::runtime_error("not yet implemented");
}

std::ostream& operator<<(std::ostream&stream, const Attribute &attribute) {
	const char* pType = attribute.type();
	if (pType == nullptr)
		return stream << "undefined";

	stream << getTypeName(pType) << " " << attribute.name() << "=";
	const char *pData = reinterpret_cast<const char*>(attribute.data());
	if (attribute.isScalar()) {
		displayValue(stream, pData, pType);
	} else {
		if (attribute.isString()) {
			stream << "'" << attribute.getString() << "'";
		} else {
			const size_t typeSize = getTypeSize(pType);
			stream << '[';
			for (size_t i = 0; i < attribute.size(); ++i, pData += typeSize) {
				if (i > 0)
					stream << ',';
				displayValue(stream, pData, pType);
			}
			stream << ']';
		}
	}
	return stream;
}
