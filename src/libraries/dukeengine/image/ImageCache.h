#ifndef IMAGECACHE_H_
#define IMAGECACHE_H_

#include "ImageHolder.h"

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>

#include <string>
#include <queue>

class ImageCache : public boost::noncopyable {
public:
	ImageCache( size_t limit );
	~ImageCache();

	/**
	 * Pushes the image holder in the cache by copying it.
	 */
	void put(const std::string &id, const ImageHolder &imageHolder);

	/**
	 * Updates the imageHolder with the cached one.
	 *
	 * returns : true if the object was in cache, false otherwise
	 */
	bool get(const std::string &id, ImageHolder &imageHolder) const;
private:
	typedef ImageHolder MAP_VALUE;
	typedef boost::unordered_map<std::string, MAP_VALUE> MAP;
	const size_t m_iSizeLimit;
	MAP m_Map;
	typedef std::queue<std::string> QUEUE;
	QUEUE m_Queue;
	unsigned long	m_CurrentMemory;

	bool evict();
};

#endif /* IMAGECACHE_H_ */
