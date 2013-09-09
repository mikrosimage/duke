#pragma once

namespace duke {

enum class ColorSpace
	: unsigned char {
		Auto, Linear, sRGB, GammaCorrected, AdobeRGB, Rec709, KodakLog, AlexaLogC, _END
};

// Deduce a the colorspace
ColorSpace resolveFromExtension(const char* pFileExtension);
ColorSpace resolveFromName(const char* pColorspace);


// GlSl functions names
const char* getToLinearFunction(const ColorSpace fromColorspace);
const char* getToScreenFunction(const ColorSpace fromColorspace);

} /* namespace duke */
