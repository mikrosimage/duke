#include <dukeengine/memory/alloc/MemoryBlock.h>
#include <dukeengine/memory/alloc/Allocators.h>
#include <dukeengine/image/ImageCache.h>

#define BOOST_TEST_MODULE ImageCache
#include <boost/test/unit_test.hpp>

#include <iostream>

using namespace ::std;
using namespace ::mikrosimage::alloc;

AlignedMallocAllocator alignedMallocAlloc;
const std::string id("id");

//--------------------------------------
// ImageCacheSuite
//--------------------------------------

BOOST_AUTO_TEST_SUITE( ImageCacheSuite )

ImageHolder getHolder(size_t size) {
	ImageHolder holder;
	ImageDescription desc;
	desc.imageDataSize = size;
	holder.setImageData( desc, MemoryBlockPtr( new MemoryBlock(&alignedMallocAlloc,size) ) );
	return holder;
}

BOOST_AUTO_TEST_CASE( isNotInCache )
{
	ImageCache cache(10);
	ImageHolder value;
	BOOST_CHECK( !cache.get(id, value) );
}

BOOST_AUTO_TEST_CASE( isInCache )
{
	ImageCache cache(10);
	cache.put(id, getHolder(1));

	ImageHolder value;
	BOOST_CHECK( cache.get(id, value) );
	BOOST_CHECK( !cache.get("unknown", value) );
}

BOOST_AUTO_TEST_CASE( normalEvict )
{
	ImageCache cache(10);
	cache.put("1", getHolder(5));
	cache.put("2", getHolder(5));

	ImageHolder value;
	BOOST_CHECK( !cache.get("1", value) );
	BOOST_CHECK( cache.get("2", value) );
}

BOOST_AUTO_TEST_CASE( cantEvict )
{
	ImageCache cache(10);
	cache.put(id, getHolder(100));

	ImageHolder value;
	BOOST_CHECK( cache.get(id, value) );
}

BOOST_AUTO_TEST_SUITE_END()
