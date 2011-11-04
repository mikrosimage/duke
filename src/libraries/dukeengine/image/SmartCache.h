#ifndef SMARTCACHE_H_
#define SMARTCACHE_H_

#include <dukeengine/image/ImageHolder.h>
#include <dukeengine/chain/RangeImpl.h>
#include <dukeengine/chain/Chain.h>
#include <boost/noncopyable.hpp>
#include <string>

class ImageDecoderFactory;
struct PlaylistHelper;

class SmartCache : private Chain {
public:
    SmartCache(uint64_t limit, const ImageDecoderFactory& factory);

    void seek(std::size_t frame, uint32_t speed, const PlaylistHelper *);

    bool get(std::size_t playlistItr, ImageHolder &imageHolder) const;

    inline bool isActive() const {
        return m_CacheSizeLimit > 0;
    }

    // reexporting visibility
    inline void dump(ForwardRange<uint64_t> & range, const uint64_t imageHash) const {
        return Chain::dump(range, imageHash);
    }
private:
    virtual size_t getNewEndIndex(const TChain&) const;

    const uint64_t m_CacheSizeLimit;
    const ImageDecoderFactory& m_ImageDecoderFactory;
    const PlaylistHelper *m_pCurrentHelper;
};

#endif /* SMARTCACHE_H_ */
