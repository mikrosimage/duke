#define BOOST_TEST_MODULE VirtualMemoryBlock
#include <boost/test/unit_test.hpp>

#include <memory/alloc/MemoryBlock.h>
#include <memory/alloc/Allocators.h>

using namespace ::mikrosimage::alloc;

BOOST_AUTO_TEST_SUITE( AllocatorSuite )

const size_t size = 1 * 1024 * 1024; // 1MB
const char PATTERN = 0x7F;

void testAlloc( Allocator* const pAllocator )
{
	char* pBuffer = pAllocator->malloc( size );

	BOOST_CHECK( pBuffer );
	pAllocator->free( pBuffer );
	BOOST_CHECK( pAllocator->alignment() > 0 );
}

void testBlockAlloc( Allocator* const pAllocator )
{
	MemoryBlock memory( pAllocator, size );
	char* pBuffer = memory.getPtr<char>();

	BOOST_CHECK( pBuffer );
	BOOST_CHECK( memory.size() == size );
}

void testAllocThenGiveOwnershipToBlock( Allocator* const pAllocator )
{
	char* pBuffer = pAllocator->malloc( size );

	BOOST_CHECK( pBuffer );
	{
		MemoryBlock memory( pAllocator, pBuffer, size );
		BOOST_CHECK( memory.size() == size );
	}
}

void testMultipleBlocHoldSameData( Allocator* const pAllocator )
{
	char* const ptr = pAllocator->malloc( 1 );

	*ptr = PATTERN;

	MemoryBlockPtr firstPtr;
	{
		MemoryBlockPtr secondPtr( new MemoryBlock( pAllocator, ptr, 1 ) );
		BOOST_CHECK( secondPtr->getPtr<char>()[0] == PATTERN );
		firstPtr = secondPtr;
	}
	BOOST_CHECK( firstPtr->getPtr<char>()[0] == PATTERN );
}

void allTest( Allocator* const pAllocator )
{
	testAlloc( pAllocator );
	testBlockAlloc( pAllocator );
	testAllocThenGiveOwnershipToBlock( pAllocator );
	testMultipleBlocHoldSameData( pAllocator );
}

AlignedMallocAllocator alignedMallocAlloc;
MallocAllocator mallocAlloc;
NewAllocator newAlloc;
#if defined ( WINDOWS )
VirtualAllocator virtualAlloc;
#endif

BOOST_AUTO_TEST_CASE( AllocatorTest )
{
	allTest( &alignedMallocAlloc );
	allTest( &mallocAlloc );
	allTest( &newAlloc );
#if defined ( WINDOWS )
	allTest( &virtualAlloc );
#endif
}

void testObjectAllocator( Allocator* allocator )
{
	char* data = allocator->malloc( 1 );

	BOOST_CHECK( data != NULL );
	data[0] = PATTERN; // check if we can write this memory
	allocator->free( data );
}

BOOST_AUTO_TEST_CASE( AllocatorObjectTest )
{
	testObjectAllocator( &alignedMallocAlloc );
	testObjectAllocator( &mallocAlloc );
	testObjectAllocator( &newAlloc );
#if defined ( WINDOWS )
	testObjectAllocator( &virtualAlloc );
#endif
}

BOOST_AUTO_TEST_SUITE_END()

