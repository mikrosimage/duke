/*
 * ShaderFactory.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef SHADERFACTORY_H_
#define SHADERFACTORY_H_

#include <duke/gl/Shader.hpp>

namespace duke {

struct ShaderDescription {
	bool swapEndianness = false;
	bool swapRedAndBlue = false;
	bool tenBitUnpack = false; ShaderDescription() =default;
	ShaderDescription(bool swapEndianness, bool swapRedAndBlue, bool tenBitUnpack) :
	swapEndianness(swapEndianness), swapRedAndBlue(swapRedAndBlue), tenBitUnpack(tenBitUnpack) {
	}
	bool operator<(const ShaderDescription &other) const {
		return hash()<other.hash();
	}
	unsigned hash()const {
		const unsigned a = swapEndianness ? 1u:0u;
		const unsigned b = swapRedAndBlue ? 2u:0u;
		const unsigned c = tenBitUnpack ? 4u:0u;
		return a|b|c;
	}
};

std::string buildFragmentShaderSource(const ShaderDescription &description);
std::string buildVertexShaderSource(const ShaderDescription &description);
SharedProgram buildProgram(const ShaderDescription &description);

} /* namespace duke */
#endif /* SHADERFACTORY_H_ */
