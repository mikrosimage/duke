#include <dukeengine/sequence/ClipHelper.h>
#include <dukeengine/sequence/PlaylistHelper.h>
#include <player.pb.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#define BOOST_TEST_MODULE ClipHelper
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

//--------------------------------------
// FrameUtilsSuite
//--------------------------------------

BOOST_AUTO_TEST_SUITE( FrameUtilsSuite )

BOOST_AUTO_TEST_CASE( existAtTest )
{
    BOOST_CHECK( !existAt( 0, 0, 0 ) );
    BOOST_CHECK( existAt( 0, 1, 0 ) );
    BOOST_CHECK( !existAt( 0, 1, 1 ) );
}

BOOST_AUTO_TEST_CASE( isRecFrameTest )
{
    ::duke::protocol::Clip clip;
    clip.set_filename( "prefix####.jpg" );
    BOOST_CHECK_EQUAL( 0u, clip.recin() );
    BOOST_CHECK_EQUAL( 0u, clip.recout() );

    BOOST_CHECK( !isRecFrame( clip, 0 ) );

    clip.set_recout( 1 );
    BOOST_CHECK( isRecFrame( clip, 0 ) );
    BOOST_CHECK( !isRecFrame( clip, 1 ) );

    clip.set_recin( 10 );
    clip.set_recout( 12 );
    BOOST_CHECK( !isRecFrame( clip, 9 ) );
    BOOST_CHECK( isRecFrame( clip, 10 ) );
    BOOST_CHECK( isRecFrame( clip, 11 ) );
    BOOST_CHECK( !isRecFrame( clip, 12 ) );
}

BOOST_AUTO_TEST_CASE( durationTest )
{
    BOOST_CHECK_EQUAL(duration(0,0),0U);
    BOOST_CHECK_EQUAL(duration(0,1),1U);
}

BOOST_AUTO_TEST_CASE( clampFrameTest )
{
    BOOST_CHECK_EQUAL(clampFrame(0,0,0), 0U);
    BOOST_CHECK_EQUAL(clampFrame(1,5,2), 2U);
    BOOST_CHECK_EQUAL(clampFrame(1,5,0), 1U);
    BOOST_CHECK_EQUAL(clampFrame(1,5,9), 5U);
}

BOOST_AUTO_TEST_CASE( offsetClampFrameTest )
{
    BOOST_CHECK_EQUAL(offsetClampFrame(0,0,0,0), 0U);
    // within range
    BOOST_CHECK_EQUAL(offsetClampFrame(1,5,2,0), 2U);
    BOOST_CHECK_EQUAL(offsetClampFrame(1,5,2,-1), 1U);
    BOOST_CHECK_EQUAL(offsetClampFrame(1,5,2,1), 3U);
    // outside range
}

BOOST_AUTO_TEST_CASE( offsetLoopFrameTest )
{
    BOOST_CHECK_EQUAL(offsetLoopFrame(0,0,0,0), 0U);
    // within range
    BOOST_CHECK_EQUAL(offsetLoopFrame(1,5,2,0), 2U);
    BOOST_CHECK_EQUAL(offsetLoopFrame(1,5,2,-1), 1U);
    BOOST_CHECK_EQUAL(offsetLoopFrame(1,5,2,1), 3U);
    // outside range
    cerr << "outside range" << endl;
    BOOST_CHECK_EQUAL(offsetLoopFrame(1,5,2,4), 1U);
    BOOST_CHECK_EQUAL(offsetLoopFrame(1,5,2,-2), 5U);
    // outside range modulo
    cerr << "outside range modulo" << endl;
    BOOST_CHECK_EQUAL(offsetLoopFrame(1,5,2,4+5), 1U);
    BOOST_CHECK_EQUAL(offsetLoopFrame(1,5,2,-2-5), 5U);
}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------
// ClipHelperSuite
//--------------------------------------
BOOST_AUTO_TEST_SUITE( ClipHelperSuite )

BOOST_AUTO_TEST_CASE( frameIndex )
{
    ::duke::protocol::Clip clip;
    clip.set_filename( "prefix####.jpg" );
    clip.set_recin( 0 );
    clip.set_recout( 1 );
    clip.set_srcin( 10 );
    ClipHelper helper( clip );
    BOOST_CHECK_EQUAL( "prefix0010.jpg", helper.getFilenameAt(0) );
}

BOOST_AUTO_TEST_CASE( filenameNotSet )
{
    ::duke::protocol::Clip clip;
    clip.set_recin( 0 );
    clip.set_recout( 1 );
    ClipHelper helper( clip );
}

BOOST_AUTO_TEST_CASE( filenameBadSet )
{
    ::duke::protocol::Clip clip;
    clip.set_recin( 0 );
    clip.set_recout( 1 );
    clip.set_filename( "t" );
    BOOST_CHECK_NO_THROW( ClipHelper helper( clip ) )
}

BOOST_AUTO_TEST_CASE( frameIndexTooLarge )
{
    ::duke::protocol::Clip clip;
    clip.set_filename( "prefix#.jpg" );
    clip.set_recin( 0 );
    clip.set_recout( 1 );
    ClipHelper helper( clip );
    BOOST_CHECK_NO_THROW( helper.getFilenameAt(5) )
    BOOST_REQUIRE_THROW( helper.getFilenameAt(10), std::logic_error )
}

BOOST_AUTO_TEST_CASE( sourceFrame )
{
    ::duke::protocol::Clip clip;
    clip.set_filename( "#" );
    clip.set_recin( 0 );
    clip.set_recout( 10 );
    clip.set_srcin( 0 );
    ClipHelper helper( clip );
    BOOST_CHECK_EQUAL( 0u, helper.getSourceFrame( 0 ) );
    BOOST_CHECK_EQUAL( 9u, helper.getSourceFrame( 9 ) );
}

BOOST_AUTO_TEST_CASE( sourceFrameOffset )
{
    ::duke::protocol::Clip clip;
    clip.set_filename( "#" );
    clip.set_recin( 0 );
    clip.set_recout( 10 );
    clip.set_srcin( 5 );
    ClipHelper helper( clip );
    BOOST_CHECK_EQUAL( 5u, helper.getSourceFrame( 0 ) );
    BOOST_CHECK_EQUAL( 14u, helper.getSourceFrame( 9 ) );
}

//BOOST_AUTO_TEST_CASE( sourceFrameHalfSpeed )
//{
//    ::duke::protocol::Clip clip;
//    clip.set_filename( "#" );
//    clip.set_srcin( 0 );
//    clip.set_srcout( 10 );//10 src frames
//    clip.set_recin( 0 );
//    clip.set_recout( 5 ); // 5 rec frames
//    ClipHelper helper( clip );
//    BOOST_CHECK_EQUAL( 0u, helper.getSourceFrame( 0 ) );
//    BOOST_CHECK_EQUAL( 10u, helper.getSourceFrame( 5 ) );
//}
//
//BOOST_AUTO_TEST_CASE( sourceFrameReverse )
//{
//    ::duke::protocol::Clip clip;
//    clip.set_filename( "#" );
//    clip.set_srcin( 10 );
//    clip.set_srcout( 0 );
//    clip.set_recin( 0 );
//    clip.set_recout( 10 );
//    ClipHelper helper( clip );
//    BOOST_CHECK_EQUAL( 9u, helper.getSourceFrame( 0 ) );
//    BOOST_CHECK_EQUAL( 0u, helper.getSourceFrame( 9 ) );
//}
//
//BOOST_AUTO_TEST_CASE( sourceFrameReverseHalf )
//{
//    ::duke::protocol::Clip clip;
//    clip.set_filename( "#" );
//    clip.set_recin( 0 );
//    clip.set_recout( 5 );
//    clip.set_srcin( 10 );
//    clip.set_srcout( 0 );
//    ClipHelper helper( clip );
//    BOOST_CHECK_EQUAL( 10u, helper.getSourceFrame( 0 ) );
//    BOOST_CHECK_EQUAL( 0u, helper.getSourceFrame( 5 ) );
//}

BOOST_AUTO_TEST_SUITE_END()

//--------------------------------------
// PlaylistHelperSuite
//--------------------------------------
BOOST_AUTO_TEST_SUITE( PlaylistHelperSuite )

::duke::protocol::Playlist buildPlaylist()
{
    ::duke::protocol::Playlist playlist;
    playlist.set_loop(true);
    ::duke::protocol::Clip* const pFirst = playlist.add_clip();
    pFirst->set_filename( "prefix####.jpg" );
    pFirst->set_recin( 10 );
    pFirst->set_recout( 20 );
    pFirst->set_srcin( 10 );
    ::duke::protocol::Clip* const pSecond = playlist.add_clip();
    pSecond->set_path( "../toto" );
    pSecond->set_filename( "prefix_####_suffix.dpx" );
    pSecond->set_recin( 15 );
    pSecond->set_recout( 30 );
    return playlist;
}

BOOST_AUTO_TEST_CASE( PlaylistHelperTests )
{
    ::duke::protocol::Playlist playlist( buildPlaylist() );
    PlaylistHelper helper( playlist );

    BOOST_CHECK_EQUAL( 20u, helper.getFrameCount() );
    BOOST_CHECK_EQUAL( 10u, helper.getRecIn() );
    BOOST_CHECK_EQUAL( 30u, helper.getRecOut() );

    BOOST_CHECK_EQUAL( 10u, helper.getClampedFrame(9) );
    BOOST_CHECK_EQUAL( 10u, helper.getClampedFrame(10) );
    BOOST_CHECK_EQUAL( 11u, helper.getClampedFrame(11) );
    BOOST_CHECK_EQUAL( 28u, helper.getClampedFrame(28) );
    BOOST_CHECK_EQUAL( 29u, helper.getClampedFrame(29) );
    BOOST_CHECK_EQUAL( 29u, helper.getClampedFrame(30) );

    BOOST_CHECK_EQUAL( 29u, helper.getWrappedFrame(9) );
    BOOST_CHECK_EQUAL( 10u, helper.getWrappedFrame(10) );
    BOOST_CHECK_EQUAL( 11u, helper.getWrappedFrame(11) );
    BOOST_CHECK_EQUAL( 28u, helper.getWrappedFrame(28) );
    BOOST_CHECK_EQUAL( 29u, helper.getWrappedFrame(29) );
    BOOST_CHECK_EQUAL( 10u, helper.getWrappedFrame(30) );

    // we are in loop mode so normalized frame is wrapped
    BOOST_CHECK_EQUAL( 29u, helper.getNormalizedFrame(9) );
    BOOST_CHECK_EQUAL( 10u, helper.getNormalizedFrame(10) );
    BOOST_CHECK_EQUAL( 11u, helper.getNormalizedFrame(11) );
    BOOST_CHECK_EQUAL( 28u, helper.getNormalizedFrame(28) );
    BOOST_CHECK_EQUAL( 29u, helper.getNormalizedFrame(29) );
    BOOST_CHECK_EQUAL( 10u, helper.getNormalizedFrame(30) );
}

BOOST_AUTO_TEST_SUITE_END()
