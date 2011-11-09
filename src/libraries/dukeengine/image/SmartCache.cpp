/*
 * SmartCache.cpp
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#include "SmartCache.h"

#include "ImageHolder.h"

#include <dukeengine/cache/CacheKey.hpp>
#include <dukeengine/cache/LookAheadCache.h>

#include <dukeengine/host/io/ImageDecoderFactory.h>
#include <dukeengine/sequence/PlaylistHelper.h>

#include <cassert>

using namespace cache;

struct SmartCacheImpl : public LookAheadCache<uint64_t, ImageHolder, CacheKey> {
    SmartCacheImpl(uint64_t limit, const ImageDecoderFactory& factory) :
        m_CacheSizeLimit(limit), m_ImageDecoderFactory(factory), m_pCurrentHelper(NULL) {
    }
    virtual void limit(Map &cache) {
    }
    virtual void workerThreadEntry() {
    }
private:
    const uint64_t m_CacheSizeLimit;
    const ImageDecoderFactory& m_ImageDecoderFactory;
    const PlaylistHelper *m_pCurrentHelper;
};

SmartCache::SmartCache(uint64_t limit, const ImageDecoderFactory& factory) :
    m_pImpl(new SmartCacheImpl(limit, factory)) {
}

SmartCache::~SmartCache() {
}

bool SmartCache::get(size_t playlistItr, ImageHolder & imageHolder) const {
    //    const uint64_t hash = helper.getHashAtIterator(itrIndex);
    //    if (hash == 0)
    //        return false;
    //
    //    const boost::filesystem::path filename = helper.getPathAtHash(hash);
    //    if (filename.empty())
    //        return true;
    //
    //    return imageReader.load(filename, value);
    return false;
}

void SmartCache::seek(size_t frame, uint32_t speed, const PlaylistHelper *pHelper) {
    assert(pHelper != NULL);
    //    m_pCurrentHelper = pHelper;
    //    if (!isActive()) // TODO if same frame don't post a new job
    //        return;
    //    using range::PlaylistFrameRange;
    //    const ptrdiff_t balancingBound = speed == 0 ? 0 : (speed > 0 ? -5 : 5);
    //    PlaylistFrameRange range(m_pCurrentHelper->getFrameCount(), frame, balancingBound, speed < 0);
    //    Chain::postNewJob<PlaylistFrameRange>(range, boost::bind(&PlaylistHelper::getPathStringAtHash, getSharedPlaylistHelper(), _1));
}

