/*
 * DiskMediaStream.h
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef DISKMEDIASTREAM_H_
#define DISKMEDIASTREAM_H_

#include "IMediaStream.h"
#include <sequence/Item.hpp>

namespace duke {

class DiskMediaStream: public duke::IMediaStream {
public:
	DiskMediaStream(const sequence::Item& item);
	virtual void generateFilePath(std::string &path, size_t atFrame) const;
private:
	sequence::Item m_Item;
};

} /* namespace duke */
#endif /* DISKMEDIASTREAM_H_ */
