#pragma once

namespace duke {

enum class ColorSpace
	: unsigned char {
		Auto, Linear, sRGB, GammaCorrected, AdobeRGB, Rec709, KodakLog, AlexaLogC, _END
};

} /* namespace duke */
