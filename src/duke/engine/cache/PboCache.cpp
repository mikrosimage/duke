/*
 * PboCache.cpp
 *
 *  Created on: Feb 11, 2013
 *      Author: Guillaume Chatelet
 */

#include "PboCache.h"
#include <duke/engine/cache/ImageCache.h>

namespace duke {

void PboCache::fill(const ImageCache& imageCache, const std::vector<MediaFrameReference>& mfrs) {
	for (const MediaFrameReference& mfr : mfrs) {
		if (has(mfr))
			continue;
		printf("next frame to serve is : stream %p and frame %lu\n", mfr.first, mfr.second);
		PackedFrame packedFrame;
		if (imageCache.get(mfr, packedFrame)) {
			const auto dataSize = packedFrame.description.dataSize;
			printf("Needing a pbo of size : %lu\n", dataSize);
			auto pSharedPbo = m_PboPool.get(dataSize);
			auto pboBound = pSharedPbo->scope_bind_buffer();
			GLubyte* ptr = (GLubyte*) glMapBufferRange(pSharedPbo->target, 0, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			memcpy(ptr, packedFrame.pData.get(), dataSize);
			glUnmapBuffer(pSharedPbo->target);
			m_Map.insert( { mfr, std::move(pSharedPbo) });
		} else {
			printf("No image in cache for now\n");
		}
	}
}

bool PboCache::has(const MediaFrameReference& mfr) const {
	return m_Map.find(mfr) != m_Map.end();
}

bool PboCache::get(const MediaFrameReference& mfr, SharedPbo &pbo) const {
	const auto& pFound = m_Map.find(mfr);
	if (pFound == m_Map.end())
		return false;
	pbo = pFound->second;
	return true;
}

} /* namespace duke */
