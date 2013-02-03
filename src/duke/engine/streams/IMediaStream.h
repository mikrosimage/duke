/*
 * MediaStream.h
 *
 *  Created on: Jan 4, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef MEDIASTREAM_H_
#define MEDIASTREAM_H_

#include <duke/NonCopyable.h>

#include <string>

namespace duke {

struct Context;
class IMediaStream: public noncopyable {
public:
	virtual ~IMediaStream() = 0;
	virtual void generateFilePath(std::string &path, size_t atFrame) const = 0;
};

} /* namespace duke */

#endif /* MEDIASTREAM_H_ */
