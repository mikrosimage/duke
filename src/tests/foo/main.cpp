#include <iostream>

#define BOOST_TEST_MODULE FooTests
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE( FooTestSuite )

BOOST_AUTO_TEST_CASE( FooTest1 )
{
    using namespace boost::unit_test::framework;
    BOOST_CHECK( master_test_suite().argc > 1);
}

BOOST_AUTO_TEST_SUITE_END()
