/*
 * DiskMediaStream.cpp
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#include "DiskMediaStream.h"

namespace duke {

DiskMediaStream::DiskMediaStream(const sequence::Item& item) :
		m_Item(item) {
}

void DiskMediaStream::generateFilePath(std::string &path, size_t atFrame) const {
	switch (m_Item.getType()) {
	case sequence::Item::INVALID:
	case sequence::Item::INDICED:
	case sequence::Item::PACKED:
		path.clear();
		break;
	case sequence::Item::SINGLE:
		path = m_Item.filename;
		break;
	}
}

} /* namespace duke */
