#include <dukeengine/memory/alloc/virtual/VirtualMemoryReserver.h>

#define BOOST_TEST_MODULE VirtualMemoryBlock
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( VirtualMemorySuite )

BOOST_AUTO_TEST_CASE( VirtualMemoryReserverTest )
{
#if defined ( WINDOWS )
	const size_t expectedPageSize( 4 * 1024 );

	{
		VirtualMemoryReserver reserver( 1 );
		BOOST_CHECK_EQUAL( expectedPageSize, reserver.getPageSize() );
		BOOST_CHECK_EQUAL( expectedPageSize, reserver.getReservedSize() );
	}
	{
		const size_t size( 10 * 1024 * 1024 );
		VirtualMemoryReserver reserver( size );
		BOOST_CHECK_EQUAL( expectedPageSize, reserver.getPageSize() );
		BOOST_CHECK_EQUAL( size, reserver.getReservedSize() );
	}
#endif
}

BOOST_AUTO_TEST_SUITE_END()

