#pragma once

#include "IMediaStream.hpp"
#include <sequence/Item.hpp>
#include <string>

namespace duke {

class DiskMediaStream: public duke::IMediaStream {
public:
	DiskMediaStream(const sequence::Item& item);

	virtual InputFrameOperationResult process(const MediaFrameReference& mfr) const override;
private:
	std::string generateFilePath(size_t atFrame) const;
	std::string writeFilename(size_t frame) const;

	sequence::Item m_Item;
	sequence::Item::Type m_ItemType;
	std::string m_Prefix;
	std::string m_Suffix;
};

} /* namespace duke */
