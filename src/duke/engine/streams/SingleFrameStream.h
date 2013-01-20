/*
 * SingleFrameStream.h
 *
 *  Created on: Jan 19, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef SINGLEFRAMESTREAM_H_
#define SINGLEFRAMESTREAM_H_

#include "MediaStream.h"
#include <duke/imageio/Attributes.h>
#include <duke/gl/Textures.h>

namespace duke {

class SingleFrameStream: public IMediaStream {
public:
	SingleFrameStream(const char *pFilename);
	virtual void doRender(const Context&) const;
	const Attributes& getAttributes() const;
private:
	std::string m_Error;
	Attributes m_Attributes;
	TextureRectangle m_RectangleTexture;
};

} /* namespace duke */
#endif /* SINGLEFRAMESTREAM_H_ */
