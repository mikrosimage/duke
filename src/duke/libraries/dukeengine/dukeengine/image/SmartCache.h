/*
 * SmartCache.h
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef SMARTCACHE_H_
#define SMARTCACHE_H_

#include <dukeengine/utils/EPlaybackState.h>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

// uint64_t is defined in <cstdint> for C++11 or in <stdint.h> for C99
#include <stdint.h> // uint64_t

struct ImageDecoderFactory;
class ImageHolder;
namespace duke {
namespace protocol {
struct PlaylistHelper;
struct MediaFrame;
}  // namespace protocol
}  // namespace duke

struct SmartCache : private boost::noncopyable {
    SmartCache(size_t threads, uint64_t limit, const ImageDecoderFactory& factory);
    ~SmartCache();
    void seek(unsigned int frame, EPlaybackState state, const duke::protocol::PlaylistHelper &);
    bool get(const duke::protocol::MediaFrame &frame, ImageHolder &imageHolder) const;
private:
    struct Impl;
    const boost::scoped_ptr<Impl> m_pImpl;
};

#endif /* SMARTCACHE_H_ */
