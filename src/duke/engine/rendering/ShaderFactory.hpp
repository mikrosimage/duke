#pragma once

#include <duke/gl/Program.hpp>
#include <duke/engine/ColorSpace.hpp>

namespace duke {

struct ShaderDescription {
	// vertex
	// fragment
	bool sampleTexture = true;
	bool displayUv = false;
	// texture
	bool grayscale = false;
	bool swapEndianness = false;
	bool swapRedAndBlue = false;
	bool tenBitUnpack = false;
	ColorSpace colorspace = ColorSpace::Auto;

	ShaderDescription();
	bool operator<(const ShaderDescription &other) const;

	static ShaderDescription createTextureDesc(bool grayscale, bool swapEndianness, bool swapRedAndBlue, bool tenBitUnpack, ColorSpace colorspace);
	static ShaderDescription createSolidDesc();
	static ShaderDescription createUvDesc();
};

std::string buildFragmentShaderSource(const ShaderDescription &description);
std::string buildVertexShaderSource(const ShaderDescription &description);
SharedProgram buildProgram(const ShaderDescription &description);

} /* namespace duke */
