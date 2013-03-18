/*
 * PackedFrameDescriptionAndAttributes.h
 *
 *  Created on: Feb 16, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <duke/imageio/PackedFrameDescription.h>
#include <duke/attributes/Attributes.h>

namespace duke {

struct PackedFrameDescriptionAndAttributes {
	PackedFrameDescription description;
	Attributes attributes;
};

} /* namespace duke */
