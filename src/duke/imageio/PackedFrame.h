/*
 * PackedFrame.h
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef PACKEDFRAME_H_
#define PACKEDFRAME_H_

#include <duke/imageio/ImageDescription.h>
#include <duke/attributes/Attributes.h>
#include <memory>

namespace duke {

struct PackedFrame {
	PackedFrameDescription description;
	Attributes attributes;
	std::shared_ptr<char> pData;
};

} /* namespace duke */
#endif /* PACKEDFRAME_H_ */
