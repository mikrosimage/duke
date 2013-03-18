/*
 * PackedFrame.h
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <duke/imageio/PackedFrameDescriptionAndAttributes.h>
#include <memory>

namespace duke {

struct RawPackedFrame: public PackedFrameDescriptionAndAttributes {
	std::shared_ptr<char> pData;
};

} /* namespace duke */
