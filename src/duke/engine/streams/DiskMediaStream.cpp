#include "DiskMediaStream.hpp"

#include <duke/base/Check.hpp>
#include <duke/base/StringUtils.hpp>
#include <duke/attributes/AttributeKeys.hpp>
#include <duke/memory/Allocator.hpp>
#include <duke/filesystem/FsUtils.hpp>
#include <duke/engine/ImageLoadUtils.hpp>
#include <duke/engine/streams/MediaFrameReference.hpp>
#include <duke/imageio/DukeIO.hpp>

#include <algorithm>

namespace duke {

namespace {

BigAlignedBlock gBigAlignedMallocator;

}  // namespace

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

// Several threads will access this function at the same time.
InputFrameOperationResult DiskMediaStream::process(const MediaFrameReference& mfr) const {
    InputFrameOperationResult result;
    result.attributes().set<attribute::File>(generateFilePath(mfr.frame).c_str());
    return duke::load(getAttributes(), [&](RawPackedFrame& packedFrame, const void* pVolatileData) {
        if(!packedFrame.pData) {
            const size_t dataSize = packedFrame.description.dataSize;
            packedFrame.pData = make_shared_memory<char>(dataSize, gBigAlignedMallocator);
            memcpy(packedFrame.pData.get(), pVolatileData, dataSize);
        }
    },std::move(result));
}

std::string DiskMediaStream::generateFilePath(size_t atFrame) const {
	switch (m_ItemType) {
	case sequence::Item::PACKED:
		return writeFilename(atFrame + m_Item.start);
	case sequence::Item::SINGLE:
		return m_Item.filename;
	default:
		return {};
	}
}

std::string DiskMediaStream::writeFilename(size_t frame) const {
    const size_t paddingSize = m_Item.padding > 0 ? m_Item.padding : digits(frame);
    const size_t bufferSize = m_Prefix.size() + paddingSize + m_Suffix.size();
    std::string path;
    path.reserve(bufferSize);
    path += m_Prefix;
    appendPaddedFrameNumber(frame, paddingSize, path);
    path += m_Suffix;
    return path;
}

} /* namespace duke */
