/*
 * Attribute.h
 *
 *  Created on: Dec 21, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include <vector>
#include <ostream>
#include <string>
#include <stdexcept>
#include <typeinfo>
#include <cstddef>
#include <cstring>

template<typename T>
inline const char* typeIdName() {
	return typeid(T).name();
}

struct Attribute {
	Attribute() :
			m_SmallData(0), m_pType(nullptr), m_Size(0) {
	}

	template<typename T, class = typename std::enable_if<std::is_fundamental<T>::value>::type>
	Attribute(const char* name, const T& value) :
			Attribute(name, reinterpret_cast<const char*>(&value), sizeof(T), typeIdName<T>()) {
	}

	Attribute(const char* name, const char* pData) :
			Attribute(name, pData, strlen(pData)) {
	}

	Attribute(const char* name, const std::string &str) :
			Attribute(name, str.c_str(), str.size()) {
	}

	template<typename T, class = typename std::enable_if<std::is_fundamental<T>::value>::type>
	Attribute(const char* name, const std::vector<T> &v) :
			Attribute(name, reinterpret_cast<const char*>(v.data()), v.size() * sizeof(T), typeIdName<const T*>(), v.size()) {
	}

	const std::string& name() const {
		return m_Name;
	}

	const char* type() const {
		return m_pType;
	}

	const void* data() const {
		if (!m_pType)
			return nullptr;
		return m_ExternalData.empty() ? reinterpret_cast<const void*>(&m_SmallData) : m_ExternalData.data();
	}

	const size_t size() const {
		return m_Size;
	}

	const size_t isScalar() const {
		return m_Size == 0;
	}

	const size_t isString() const {
		return m_Size != 0 && m_pType == typeIdName<const char*>();
	}

	template<typename T>
	const T& getScalar() const {
		if (m_pType != typeid(T).name() || !isScalar())
			throw std::bad_cast();
		return *reinterpret_cast<const T*>(data());
	}

	template<typename T>
	struct TypedVectorAttribute {
		TypedVectorAttribute(const Attribute *pAttribute) :
				m_pAttribute(pAttribute) {
			if (m_pAttribute->m_pType != typeIdName<const T*>() || m_pAttribute->isScalar())
				throw std::bad_cast();
		}
		const T* begin() const {
			return reinterpret_cast<const T*>(m_pAttribute->data());
		}
		const T* end() const {
			return begin() + m_pAttribute->m_Size;
		}
	private:
		const Attribute * const m_pAttribute;
	};

	template<typename T>
	TypedVectorAttribute<T> getVector() const {
		return TypedVectorAttribute<T>(this);
	}

	const char* getString() const {
		return getVector<char>().begin();
	}

	friend void swap(Attribute &a, Attribute &b) {
		std::swap(a.m_Name, b.m_Name);
		std::swap(a.m_ExternalData, b.m_ExternalData);
		std::swap(a.m_SmallData, b.m_SmallData);
		std::swap(a.m_pType, b.m_pType);
		std::swap(a.m_Size, b.m_Size);
	}

private:
	Attribute(const char* name, const char* pString, const size_t len) :
			Attribute(name, pString, len + 1, typeIdName<const char*>(), len) {
	}
	Attribute(const char* name, const char* pData, const size_t dataSize, const char* pType, size_t count = 0) :
			m_Name(name), m_SmallData(0), m_pType(pType), m_Size(count) {
		const bool isSmall = dataSize <= sizeof(m_SmallData);
		if (!isSmall)
			m_ExternalData.resize(dataSize);
		char* pDst = isSmall ? reinterpret_cast<char*>(&m_SmallData) : m_ExternalData.data();
		memcpy(pDst, pData, dataSize);
	}
	std::string m_Name;
	std::vector<char> m_ExternalData;
	ptrdiff_t m_SmallData;
	const char* m_pType;
	size_t m_Size;
};

std::ostream& operator<<(std::ostream&, const Attribute&);

#endif /* ATTRIBUTE_H_ */
