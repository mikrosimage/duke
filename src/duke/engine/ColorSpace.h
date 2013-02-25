/*
 * ColorSpace.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef COLORSPACE_H_
#define COLORSPACE_H_

namespace duke {

enum class ColorSpace
	: unsigned char {
		Auto, Linear, sRGB, GammaCorrected, AdobeRGB, Rec709, KodakLog, _END
};

} /* namespace duke */
#endif /* COLORSPACE_H_ */
