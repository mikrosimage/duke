/*
 * IRenderer.h
 *
 *  Created on: Jan 6, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef IRENDERER_H_
#define IRENDERER_H_

#include <duke/NonCopyable.h>

namespace duke {

class IRenderer: public noncopyable {
public:
	virtual ~IRenderer() = 0;
};

} /* namespace duke */
#endif /* IRENDERER_H_ */
