/*
 * PboPackedFrame.h
 *
 *  Created on: Feb 16, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef PBOPACKEDFRAME_H_
#define PBOPACKEDFRAME_H_

#include <duke/imageio/PackedFrameDescriptionAndAttributes.h>
#include <duke/gl/GlObjects.h>
#include <memory>

namespace duke {

struct PboPackedFrame: public PackedFrameDescriptionAndAttributes {
	PboPackedFrame() = default;
	PboPackedFrame(const PackedFrameDescriptionAndAttributes &other) :
			PackedFrameDescriptionAndAttributes(other) {
	}
	std::shared_ptr<gl::GlStreamUploadPbo> pPbo;
};

} /* namespace duke */

#endif /* PBOPACKEDFRAME_H_ */
