/*
 * PackedFrame.h
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef PACKEDFRAME_H_
#define PACKEDFRAME_H_

#include <duke/imageio/PackedFrameDescriptionAndAttributes.h>
#include <memory>

namespace duke {

struct RawPackedFrame: public PackedFrameDescriptionAndAttributes {
	std::shared_ptr<char> pData;
};

} /* namespace duke */
#endif /* PACKEDFRAME_H_ */
