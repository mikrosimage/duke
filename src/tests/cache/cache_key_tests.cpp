#include <dukeengine/cache/JobPriority.hpp>

#define BOOST_TEST_MODULE KeyTestModule
#include <boost/test/unit_test.hpp>

using namespace cache;
using namespace std;

BOOST_AUTO_TEST_SUITE( KeyTestSuite )

/**
 * We must ensure that the most recent job comes first
 * Then keys are ordered by index
 */
BOOST_AUTO_TEST_CASE( keyComparison )
{
    const JobPriority _0_2(0,2);
    const JobPriority _1_0(1,0);
    const JobPriority _0_3(0,3);
    const JobPriority _0_0(0,0);
    const JobPriority _1_12(1,12);

    vector<JobPriority> sortedKeys;
    sortedKeys.push_back(_1_0);
    sortedKeys.push_back(_1_12);
    sortedKeys.push_back(_0_0);
    sortedKeys.push_back(_0_2);
    sortedKeys.push_back(_0_3);

    vector<JobPriority> shuffled(sortedKeys);
    // shuffling
    random_shuffle(shuffled.begin(), shuffled.end());

    //sorting
    sort(shuffled.begin(), shuffled.end());

    BOOST_CHECK_EQUAL_COLLECTIONS( sortedKeys.begin(), sortedKeys.end(), shuffled.begin(), shuffled.end());
}

BOOST_AUTO_TEST_SUITE_END()
