/*
 * DiskMediaStream.cpp
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#include "DiskMediaStream.h"
#include <algorithm>

namespace duke {

DiskMediaStream::DiskMediaStream(const sequence::Item& item) :
		m_Item(item), m_ItemType(m_Item.getType()) {
	if (m_ItemType == sequence::Item::PACKED || m_ItemType == sequence::Item::INDICED) {
		const auto& filename = m_Item.filename;
		const auto begin = filename.begin();
		auto firstSharpIndex = filename.find('#');
		m_Prefix = std::string(begin, begin + firstSharpIndex);
		auto lastSharpIndex = m_Item.filename.rfind('#');
		m_Suffix = std::string(begin + lastSharpIndex + 1, filename.end());
	}
}

void DiskMediaStream::generateFilePath(std::string &path, size_t atFrame) const {
	switch (m_ItemType) {
	case sequence::Item::INVALID:
	case sequence::Item::INDICED:
		path.clear();
		break;
	case sequence::Item::PACKED:
		writeFilename(path, atFrame + m_Item.start);
		break;
	case sequence::Item::SINGLE:
		path = m_Item.filename;
		break;
	}
}

void DiskMediaStream::writeFilename(std::string &path, size_t frame) const {
	path = m_Prefix;
	size_t padding = 0;
	for (; frame != 0; frame /= 10, ++padding)
		path.push_back('0' + (frame % 10));
	if (m_Item.padding > 0)
		for (; padding < (size_t) m_Item.padding; ++padding)
			path.push_back('0');
	std::reverse(path.begin() + path.size() - padding, path.end());
	path += m_Suffix;
}

} /* namespace duke */
