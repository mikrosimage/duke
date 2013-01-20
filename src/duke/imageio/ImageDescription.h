/*
 * ImageDescription.h
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef PACKEDFRAMEESCRIPTION_H_
#define PACKEDFRAMEESCRIPTION_H_

#include <cstddef>

struct PackedFrameDescription {
	size_t width, height;
	size_t glPackFormat; // corresponds to OpenGL internal image format
	size_t dataSize;
	bool swapEndianness;
	bool swapRedAndBlue;
	PackedFrameDescription() :
			width(0), height(0), glPackFormat(0), dataSize(0), swapEndianness(false), swapRedAndBlue(false) {
	}
	bool operator<(const PackedFrameDescription &other) const {
		return width < other.width && //
				height < other.height && //
				glPackFormat < other.glPackFormat && //
				dataSize < other.dataSize && //
				swapEndianness < other.swapEndianness && //
				swapRedAndBlue < other.swapRedAndBlue;
	}
};

#endif /* PACKEDFRAMEESCRIPTION_H_ */
