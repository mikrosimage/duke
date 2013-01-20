/*
 * Frame.h
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <duke/imageio/ImageDescription.h>
#include <duke/imageio/Attributes.h>
#include <memory>
#include <string>

namespace duke {

struct Frame {
	PackedFrameDescription description;
	Attributes attributes;
	std::shared_ptr<void> pData;
};

} /* namespace duke */
#endif /* FRAME_H_ */
