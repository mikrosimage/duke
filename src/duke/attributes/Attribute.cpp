#include "Attribute.hpp"

const char * getTypeName(PrimitiveType type) {
	switch (type) {
	case PrimitiveType::UNKNOWN:
		return "unknown";
	case PrimitiveType::CHAR:
		return "char";
	case PrimitiveType::UCHAR:
		return "unsigned char";
	case PrimitiveType::SHORT:
		return "short";
	case PrimitiveType::USHORT:
		return "unsigned short";
	case PrimitiveType::INT:
		return "int";
	case PrimitiveType::UINT:
		return "unsigned int";
	case PrimitiveType::LONG:
		return "long";
	case PrimitiveType::ULONG:
		return "unsigned long";
	case PrimitiveType::LONGLONG:
		return "long long";
	case PrimitiveType::ULONGLONG:
		return "unsigned long long";
	case PrimitiveType::FLOAT:
		return "float";
	case PrimitiveType::DOUBLE:
		return "double";
	default:
		throw std::runtime_error("Not yet implemented");
	}
}

size_t typeSize(PrimitiveType type) {
	switch (type) {
	case PrimitiveType::CHAR:
	case PrimitiveType::UCHAR:
		return sizeof(char);
	case PrimitiveType::SHORT:
	case PrimitiveType::USHORT:
		return sizeof(short);
	case PrimitiveType::INT:
	case PrimitiveType::UINT:
		return sizeof(int);
	case PrimitiveType::LONG:
	case PrimitiveType::ULONG:
		return sizeof(long);
	case PrimitiveType::LONGLONG:
	case PrimitiveType::ULONGLONG:
		return sizeof(long long);
	case PrimitiveType::FLOAT:
		return sizeof(float);
	case PrimitiveType::DOUBLE:
		return sizeof(double);
	case PrimitiveType::UNKNOWN:
	default:
		throw std::runtime_error("bad state");
	}
}
std::ostream& displayValue(std::ostream &stream, const char *pData,
		PrimitiveType type) {
	switch (type) {
	case PrimitiveType::CHAR:
		return stream << *reinterpret_cast<const char*>(pData);
	case PrimitiveType::UCHAR:
		return stream << *reinterpret_cast<const unsigned char*>(pData);
	case PrimitiveType::SHORT:
		return stream << *reinterpret_cast<const short*>(pData);
	case PrimitiveType::USHORT:
		return stream << *reinterpret_cast<const unsigned short*>(pData);
	case PrimitiveType::INT:
		return stream << *reinterpret_cast<const int*>(pData);
	case PrimitiveType::UINT:
		return stream << *reinterpret_cast<const unsigned int*>(pData);
	case PrimitiveType::LONG:
		return stream << *reinterpret_cast<const long*>(pData);
	case PrimitiveType::ULONG:
		return stream << *reinterpret_cast<const unsigned long*>(pData);
	case PrimitiveType::LONGLONG:
		return stream << *reinterpret_cast<const long long*>(pData);
	case PrimitiveType::ULONGLONG:
		return stream << *reinterpret_cast<const unsigned long long*>(pData);
	case PrimitiveType::FLOAT:
		return stream << *reinterpret_cast<const float*>(pData);
	case PrimitiveType::DOUBLE:
		return stream << *reinterpret_cast<const double*>(pData);
	default:
	case PrimitiveType::UNKNOWN:
		return stream;
	}
}

std::ostream& operator<<(std::ostream&stream, PrimitiveType type) {
	return stream << getTypeName(type);
}

std::ostream& operator<<(std::ostream&stream, const Attribute &attribute) {
	const PrimitiveType type = attribute.type();
	if (type == PrimitiveType::UNKNOWN)
		return stream << "undefined";
	const char *pData = reinterpret_cast<const char*>(attribute.data());
	if (attribute.isScalar()) {
		stream << type << " " << attribute.name() << "=";
		displayValue(stream, pData, type);
	} else {
		if (attribute.isString()) {
			stream << "string " << attribute.name() << "=";
			stream << "'" << attribute.getString() << "'";
		} else {
			stream << "vector<"<< type << "> " << attribute.name() << "=";
			const size_t size = typeSize(type);
			stream << '[';
			for (size_t i = 0; i < attribute.size(); ++i, pData += size) {
				if (i > 0)
					stream << ',';
				displayValue(stream, pData, type);
			}
			stream << ']';
		}
	}
	return stream;
}
