/*
 * ShaderFactory.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef SHADERFACTORY_H_
#define SHADERFACTORY_H_

#include <duke/gl/Shader.hpp>
#include <duke/engine/ColorSpace.h>

namespace duke {

struct ShaderDescription {
	bool swapEndianness = false;
	bool swapRedAndBlue = false;
	bool tenBitUnpack = false;
	ColorSpace colorspace = ColorSpace::Source;
	ShaderDescription(bool swapEndianness, bool swapRedAndBlue, bool tenBitUnpack, ColorSpace colorspace) :
	swapEndianness(swapEndianness), swapRedAndBlue(swapRedAndBlue), tenBitUnpack(tenBitUnpack), colorspace(colorspace) {
	}
	bool operator<(const ShaderDescription &other) const {
		return hash()<other.hash();
	}
	unsigned hash()const {
		const unsigned a = swapEndianness ? 1u:0u;
		const unsigned b = swapRedAndBlue ? 2u:0u;
		const unsigned c = tenBitUnpack ? 4u:0u;
		const unsigned d = (unsigned)(colorspace) << 4u;
		return a|b|c|d;
	}
};

std::string buildFragmentShaderSource(const ShaderDescription &description);
std::string buildVertexShaderSource(const ShaderDescription &description);
SharedProgram buildProgram(const ShaderDescription &description);

} /* namespace duke */
#endif /* SHADERFACTORY_H_ */
