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
#include <type_traits>

enum class PrimitiveType  : unsigned char { UNKNOWN, CHAR, UCHAR, SHORT, USHORT, INT, UINT, LONG, ULONG, LONGLONG, ULONGLONG, FLOAT, DOUBLE };
template<typename T> struct ptraits {};
template<> struct ptraits<char> {constexpr static PrimitiveType value = PrimitiveType::CHAR;};
template<> struct ptraits<unsigned char> {constexpr static PrimitiveType value = PrimitiveType::UCHAR;};
template<> struct ptraits<short> {constexpr static PrimitiveType value = PrimitiveType::SHORT;};
template<> struct ptraits<unsigned short> {constexpr static PrimitiveType value = PrimitiveType::USHORT;};
template<> struct ptraits<int> {constexpr static PrimitiveType value = PrimitiveType::INT;};
template<> struct ptraits<unsigned> {constexpr static PrimitiveType value = PrimitiveType::UINT;};
template<> struct ptraits<long long> {constexpr static PrimitiveType value = PrimitiveType::LONGLONG;};
template<> struct ptraits<unsigned long long> {constexpr static PrimitiveType value = PrimitiveType::ULONGLONG;};
template<> struct ptraits<float> {constexpr static PrimitiveType value = PrimitiveType::FLOAT;};
template<> struct ptraits<double> {constexpr static PrimitiveType value = PrimitiveType::DOUBLE;};

struct Attribute {
private:
	Attribute(const char* name, const char* pString, const size_t len) :
			Attribute(name, pString, len + 1, PrimitiveType::CHAR,len) {
	}
private:
	std::string m_Name;
	std::vector<char> m_ExternalData;
	size_t m_SmallData;
	PrimitiveType m_Type;
	size_t m_Size;
public:
	Attribute() :  m_SmallData(0), m_Type(PrimitiveType::UNKNOWN),m_Size(0) {}

	template<typename T, class = typename std::enable_if<std::is_fundamental<T>::value>::type>
	Attribute(const char* name, T value) :
			Attribute(name, reinterpret_cast<const char*>(&value), sizeof(T), ptraits<T>::value,0) {
	}

	template<typename T, class = typename std::enable_if<std::is_fundamental<T>::value>::type>
	Attribute(const char* name, const std::vector<T> &v) :
			Attribute(name, reinterpret_cast<const char*>(v.data()), v.size() * sizeof(T), ptraits<T>::value, v.size()) {
	}

	Attribute(const char* name, const char* pData) :
			Attribute(name, pData, strlen(pData)) {
	}

	Attribute(const char* name, const std::string &str) :
			Attribute(name, str.c_str(), str.size()) {
	}
	explicit Attribute(const char* name, const char* pData, const size_t dataSize, PrimitiveType type, size_t len) :
			m_Name(name), m_SmallData(0), m_Type(type), m_Size(len) {
		const bool isSmall = dataSize <= sizeof(m_SmallData);
		if (!isSmall)
			m_ExternalData.resize(dataSize);
		char* pDst = isSmall ? reinterpret_cast<char*>(&m_SmallData) : m_ExternalData.data();
		memcpy(pDst, pData, dataSize);
	}

	const std::string& name() const {
		return m_Name;
	}

	PrimitiveType type() const {
		return m_Type;
	}

	const void* data() const {
		if (m_Type == PrimitiveType::UNKNOWN)
			return nullptr;
		return m_ExternalData.empty() ? reinterpret_cast<const void*>(&m_SmallData) : m_ExternalData.data();
	}

	const size_t size() const {
		return m_Size;
	}

	const bool isScalar() const {
		return m_Size == 0;
	}

	const bool isVector() const {
		return !isScalar();
	}

	const bool isString() const {
		return isVector() && m_Type == PrimitiveType::CHAR;
	}

	template<typename T>
	const T& getScalar() const {
		if (ptraits<T>::value!=m_Type || isVector())
			throw std::bad_cast();
		return *reinterpret_cast<const T*>(data());
	}

	template<typename T>
	struct TypedVectorAttribute {
		TypedVectorAttribute(const Attribute *pAttribute) :
				m_pAttribute(pAttribute) {
			if (m_pAttribute->m_Type != ptraits<T>::value || m_pAttribute->isScalar())
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
		std::swap(a.m_Type, b.m_Type);
		std::swap(a.m_Size, b.m_Size);
	}
};

std::ostream& operator<<(std::ostream&stream, PrimitiveType type);
std::ostream& operator<<(std::ostream&, const Attribute&);

#endif /* ATTRIBUTE_H_ */
