#define BOOST_TEST_MODULE MemoryUtils
#include <boost/test/unit_test.hpp>

#include <memory/MemoryUtils.h>
BOOST_AUTO_TEST_SUITE( MemoryUtils )

#define STEP 512U

BOOST_AUTO_TEST_CASE( roundingTest )
{
	const size_t step( STEP );

	BOOST_CHECK_EQUAL( 0u, roundDown<size_t>( 0, step ) );
	BOOST_CHECK_EQUAL( 0u, roundDown<size_t>( step - 1, step ) );
	BOOST_CHECK_EQUAL( step, roundDown<size_t>( step, step ) );
	BOOST_CHECK_EQUAL( step, roundDown<size_t>( step + 1, step ) );
	BOOST_CHECK_EQUAL( step, roundDown<size_t>( 2 * step - 1, step ) );
	BOOST_CHECK_EQUAL( 2 * step, roundDown<size_t>( 2 * step, step ) );
	BOOST_CHECK_EQUAL( 2 * step, roundDown<size_t>( 2 * step + 1, step ) );
	BOOST_CHECK_EQUAL( 0u, roundUp<size_t>( 0, step ) );
	BOOST_CHECK_EQUAL( step, roundUp<size_t>( step - 1, step ) );
	BOOST_CHECK_EQUAL( step, roundUp<size_t>( step, step ) );
	BOOST_CHECK_EQUAL( 2 * step, roundUp<size_t>( step + 1, step ) );
	BOOST_CHECK_EQUAL( 2 * step, roundUp<size_t>( 2 * step - 1, step ) );
	BOOST_CHECK_EQUAL( 2 * step, roundUp<size_t>( 2 * step, step ) );
	BOOST_CHECK_EQUAL( 3 * step, roundUp<size_t>( 2 * step + 1, step ) );
}

BOOST_AUTO_TEST_CASE( FixedRoundingTest )
{
	BOOST_CHECK_EQUAL( 0u, ( roundDown<size_t, STEP>( 0 ) ) );
	BOOST_CHECK_EQUAL( 0u, ( roundDown<size_t, STEP>( STEP - 1 ) ) );
	BOOST_CHECK_EQUAL( STEP, ( roundDown<size_t, STEP>( STEP ) ) );
	BOOST_CHECK_EQUAL( STEP, ( roundDown<size_t, STEP>( STEP + 1 ) ) );
	BOOST_CHECK_EQUAL( STEP, ( roundDown<size_t, STEP>( 2 * STEP - 1 ) ) );
	BOOST_CHECK_EQUAL( 2 * STEP, ( roundDown<size_t, STEP>( 2 * STEP ) ) );
	BOOST_CHECK_EQUAL( 2 * STEP, ( roundDown<size_t, STEP>( 2 * STEP + 1 ) ) );
	BOOST_CHECK_EQUAL( 0u, ( roundUp<size_t, STEP>( 0 ) ) );
	BOOST_CHECK_EQUAL( STEP, ( roundUp<size_t, STEP>( STEP - 1 ) ) );
	BOOST_CHECK_EQUAL( STEP, ( roundUp<size_t, STEP>( STEP ) ) );
	BOOST_CHECK_EQUAL( 2 * STEP, ( roundUp<size_t, STEP>( STEP + 1 ) ) );
	BOOST_CHECK_EQUAL( 2 * STEP, ( roundUp<size_t, STEP>( 2 * STEP - 1 ) ) );
	BOOST_CHECK_EQUAL( 2 * STEP, ( roundUp<size_t, STEP>( 2 * STEP ) ) );
	BOOST_CHECK_EQUAL( 3 * STEP, ( roundUp<size_t, STEP>( 2 * STEP + 1 ) ) );
}

BOOST_AUTO_TEST_SUITE_END()
