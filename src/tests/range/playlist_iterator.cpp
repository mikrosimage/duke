#include <player.pb.h>

#include <dukeengine/image/PlaylistIterator.h>
#include <iostream>

#define BOOST_TEST_MODULE PlaylistIteratorTest
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE( PlaylistIteratorSuite )

string doStep(PlaylistIterator& itr, const PlaylistHelper& helper) {
    boost::filesystem::path path( helper.getPathAtHash(itr.front()));
    itr.popFront();
    return path.string();
}

BOOST_AUTO_TEST_CASE( playlistIterator0 )
{
    ::duke::protocol::Playlist playlist;
    const auto pClip = playlist.add_clip();
    pClip->set_filename("f####");
    pClip->set_srcin(1000);
    pClip->set_recin(1000);
    pClip->set_recout(1010);

    PlaylistHelper helper(playlist);

    BOOST_CHECK_THROW(PlaylistIterator(helper,0,0), std::runtime_error);

    {
        const int32_t frame = 1005;
        const int32_t speed = 1; // forward
        PlaylistIterator itr(helper,frame,speed);
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1005");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1006");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1007");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1008");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1009");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1000");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1001");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1002");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1003");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1004");
        BOOST_CHECK(itr.empty());
    }
    {
        const int32_t frame = 1005;
        const int32_t speed = 0; // stopped
        PlaylistIterator itr(helper,frame,speed);
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1005");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1006");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1004");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1007");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1003");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1008");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1002");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1009");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1001");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1000");
        BOOST_CHECK(itr.empty());
    }
    {
        const int32_t frame = 1005;
        const int32_t speed = -1; // reverse
        PlaylistIterator itr(helper,frame,speed);
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1005");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1004");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1003");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1002");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1001");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1000");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1009");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1008");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1007");
        BOOST_CHECK_EQUAL(doStep(itr, helper), "f1006");
        BOOST_CHECK(itr.empty());
    }
}

BOOST_AUTO_TEST_SUITE_END()
