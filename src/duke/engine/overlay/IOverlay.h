/*
 * IOverlay.h
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <duke/NonCopyable.h>

namespace duke {

struct Context;
class IOverlay: public noncopyable {
public:
	virtual ~IOverlay() = 0;
	virtual void render(const Context&) const =0;
};

} /* namespace duke */
