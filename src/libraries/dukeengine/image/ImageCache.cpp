/*
 * ImageCache.cpp
 *
 *  Created on: 10 sept. 2010
 *      Author: Guillaume Chatelet
 */

#include "ImageCache.h"
#include <iostream>

ImageCache::ImageCache( size_t limit )  : m_iSizeLimit(limit), m_CurrentMemory(0) {
}

ImageCache::~ImageCache() {
#ifdef DEBUG
	std::cout << "ImageCache : releasing " << m_Queue.size() << " images ( " << m_CurrentMemory << " B )" << std::endl;
#endif
}

void ImageCache::put(const std::string &id, const ImageHolder &imageHolder){
	const size_t incomingDataSize = imageHolder.getImageDataSize();

	// making room for the incoming element
	while( m_CurrentMemory + incomingDataSize >= m_iSizeLimit )
		if(!evict())
			break;

	// pushing the element in the map
	m_Map[id] = imageHolder;

	// updating eviction statistics
	m_CurrentMemory += incomingDataSize;
	m_Queue.push(id);
}

bool ImageCache::evict(){
	if(m_Queue.empty())
		return false;
	const MAP::iterator pItr = m_Map.find( m_Queue.front() );
	assert( pItr != m_Map.end() );
	m_CurrentMemory -= pItr->second.getImageDataSize();
	m_Map.erase( pItr );
	m_Queue.pop();
	return true;
}

bool ImageCache::get(const std::string &id, ImageHolder &imageHolder) const {
	const MAP::const_iterator pValue = m_Map.find(id);
	if(pValue == m_Map.end())
		return false;

	// copying the result in the reference
	imageHolder = pValue->second;
	return true;
}
