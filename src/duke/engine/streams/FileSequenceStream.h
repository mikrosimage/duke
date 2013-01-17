/*
 * FileSequenceStream.h
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef FILESEQUENCESTREAM_H_
#define FILESEQUENCESTREAM_H_

#include "MediaStream.h"
#include <duke/engine/LoadableTexture.h>

#include <memory>

namespace duke {

class ImageRenderer;
class FileSequenceStream: public duke::MediaStream {
public:
	FileSequenceStream(const std::shared_ptr<ImageRenderer> &pRenderer, const char* filename);
	virtual void doRender(const Context&) const;
	const Attributes& getAttributes() const;
private:
	const std::shared_ptr<ImageRenderer> m_pRenderer;
	const size_t m_Frame = 0;
	LoadableTexture m_Texture;
};

} /* namespace duke */
#endif /* FILESEQUENCESTREAM_H_ */
