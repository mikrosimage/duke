#ifndef SMARTCACHE_H_
#define SMARTCACHE_H_

#include <dukeengine/image/ImageHolder.h>
#include <dukeengine/chain/RangeImpl.h>
#include <dukeengine/chain/Chain.h>
#include <boost/noncopyable.hpp>
#include <string>

class ImageDecoderFactory;

class SmartCache : public boost::noncopyable {
public:
	SmartCache( size_t limit, const ImageDecoderFactory& factory);
	~SmartCache();

	inline const bool isRunning() const {return !m_Terminate;}
	void seek(ForwardRange<uint64_t> &range, const Chain::HashToFilenameFunction &function);
	bool get(const uint64_t &hash, ImageHolder &imageHolder) const;
	void dump(ForwardRange<uint64_t>& range, const uint64_t &current) const;

private:
	bool computeTotalWeight(const TChain& _tchain, uint64_t& totalSize);
	void load(Chain& _c, const ImageDecoderFactory& factory);
	void decode(Chain& _c, const ImageDecoderFactory& factory);
	void loadAndDecode(Chain& _c, const ImageDecoderFactory& factory);

private:
	const size_t m_iSizeLimit;
	uint64_t m_CurrentMemory;
	bool m_Terminate;

    // All the following objects have to be initialized in this order
    boost::mutex m_CacheStateMutex;
    boost::mutex m_LoadingMutex;
    boost::condition_variable m_LoadingCondition;
	Chain m_Chain;
};

#endif /* SMARTCACHE_H_ */
