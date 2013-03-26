#pragma once

namespace duke {

enum class ColorSpace
	: unsigned char {
		Auto, Linear, sRGB, GammaCorrected, AdobeRGB, Rec709, KodakLog, _END
};

} /* namespace duke */
