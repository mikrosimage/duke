#include <dukeapi/protobuf_builder/CmdLineParser.h>
#include <dukeengine/CmdLineOptions.h>

#define BOOST_TEST_MODULE CmdLineOptions
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE( CmdLineCacheOptionsTestSuite )

BOOST_AUTO_TEST_CASE( cacheEmpty ) {
    BOOST_CHECK_THROW( parseCache(""), cmdline_exception );
}

BOOST_AUTO_TEST_CASE( erroneousValues ) {
    BOOST_CHECK_THROW( parseCache("kjdc"), cmdline_exception );
    BOOST_CHECK_THROW( parseCache("0$"), cmdline_exception );
    BOOST_CHECK_THROW( parseCache("110%"), cmdline_exception );
}

BOOST_AUTO_TEST_CASE( cacheNumericValue ) {
    const static unsigned long long Kilo = 1024;
    const static unsigned long long Mega = Kilo*Kilo;
    const static unsigned long long Giga = Kilo*Mega;
    BOOST_CHECK_EQUAL( parseCache("5"), 5*Mega );
    BOOST_CHECK_EQUAL( parseCache("5M"), 5*Mega );
    BOOST_CHECK_EQUAL( parseCache("5m"), 5*Mega );
    BOOST_CHECK_EQUAL( parseCache("1G"), Giga );
    BOOST_CHECK_EQUAL( parseCache("1g"), Giga );
    BOOST_CHECK( parseCache("50%")>0 );
}

BOOST_AUTO_TEST_SUITE_END()
