/*
 * FileInfoOverlay.h
 *
 *  Created on: Jan 9, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef FILEINFOOVERLAY_H_
#define FILEINFOOVERLAY_H_

#include "MediaStream.h"

#include <memory>

namespace duke {

struct GlyphRenderer;
class SingleFrameStream;

class FileInfoOverlay: public duke::IMediaStream {
public:
	FileInfoOverlay(const std::shared_ptr<GlyphRenderer>& pTextRenderer, const std::shared_ptr<SingleFrameStream>& pFileStream);
	virtual void doRender(const Context&) const;
private:
	const std::shared_ptr<GlyphRenderer> m_pRenderer;
	const std::shared_ptr<SingleFrameStream> m_pFileStream;
};

} /* namespace duke */
#endif /* FILEINFOOVERLAY_H_ */
