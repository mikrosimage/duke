/*
 * SmartCache.h
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef SMARTCACHE_H_
#define SMARTCACHE_H_

#include <boost/noncopyable.hpp>

#include <cinttypes>
#include <memory>

struct ImageDecoderFactory;
struct PlaylistHelper;
class ImageHolder;

struct SmartCacheImpl; // PImpl idiom

struct SmartCache : private boost::noncopyable {
    SmartCache(uint64_t limit, const ImageDecoderFactory& factory);
    void seek(std::size_t frame, uint32_t speed, const PlaylistHelper &);
    bool get(uint64_t hash, ImageHolder &imageHolder) const;
private:
    const std::auto_ptr<SmartCacheImpl> m_pImpl;
};

#endif /* SMARTCACHE_H_ */
