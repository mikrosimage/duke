/*
 * PackedFrameDescriptionAndAttributes.h
 *
 *  Created on: Feb 16, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef PACKEDFRAMEDESCRIPTIONANDATTRIBUTES_H_
#define PACKEDFRAMEDESCRIPTIONANDATTRIBUTES_H_

#include <duke/imageio/PackedFrameDescription.h>
#include <duke/attributes/Attributes.h>

namespace duke {

struct PackedFrameDescriptionAndAttributes {
	PackedFrameDescription description;
	Attributes attributes;
};

} /* namespace duke */

#endif /* PACKEDFRAMEDESCRIPTIONANDATTRIBUTES_H_ */
