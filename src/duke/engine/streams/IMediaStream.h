/*
 * MediaStream.h
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <duke/NonCopyable.h>

#include <string>

namespace duke {

class IMediaStream: public noncopyable {
public:
	virtual ~IMediaStream() = 0;
	virtual void generateFilePath(std::string &path, size_t atFrame) const = 0;
};

} /* namespace duke */
