/*
 * DiskMediaStream.h
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include "IMediaStream.h"
#include <sequence/Item.hpp>
#include <string>

namespace duke {

class DiskMediaStream: public duke::IMediaStream {
public:
	DiskMediaStream(const sequence::Item& item);
	virtual void generateFilePath(std::string &path, size_t atFrame) const;
private:
	void writeFilename(std::string &path, size_t frame) const;

	sequence::Item m_Item;
	sequence::Item::Type m_ItemType;
	std::string m_Prefix;
	std::string m_Suffix;
};

} /* namespace duke */
