/*
 * ColorSpace.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

namespace duke {

enum class ColorSpace
	: unsigned char {
		Auto, Linear, sRGB, GammaCorrected, AdobeRGB, Rec709, KodakLog, _END
};

} /* namespace duke */
