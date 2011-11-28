/*
 * SmartCache.h
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef SMARTCACHE_H_
#define SMARTCACHE_H_

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

// uint64_t is defined in <cstdint> for C++11 or in <stdint.h> for C99
#include <stdint.h> // uint64_t

struct ImageDecoderFactory;
struct PlaylistHelper;
class ImageHolder;

struct SmartCache : private boost::noncopyable {
    SmartCache(size_t threads, uint64_t limit, const ImageDecoderFactory& factory);
    ~SmartCache();
    void seek(const std::size_t frame, const uint32_t speed, const PlaylistHelper &);
    bool get(uint64_t hash, ImageHolder &imageHolder) const;
private:
    struct Impl;
    const boost::scoped_ptr<Impl> m_pImpl;
};

#endif /* SMARTCACHE_H_ */
