/*
 * main.cpp
 *
 *  Created on: 14 avr. 2010
 *      Author: Guillaume Chatelet
 */

#include <sequence/PlaylistHelper.h>

#include <boost/filesystem.hpp>

#include <iostream>

#define BOOST_TEST_MODULE PlaylistIterator
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( PlaylistIteratorSuite )

::protocol::duke::Playlist buildOneFramePlaylist()
{
    ::protocol::duke::Playlist playlist;
    ::protocol::duke::Clip* const pFirst = playlist.add_clip();
    pFirst->set_filename( "prefix####.jpg" );
    pFirst->set_recin( 10 );
    pFirst->set_recout( 11 );
    pFirst->set_srcin( 10 );
    return playlist;
}

::protocol::duke::Playlist buildPlaylist()
{
    ::protocol::duke::Playlist playlist;
    playlist.set_loop(true);
    ::protocol::duke::Clip* const pFirst = playlist.add_clip();
    pFirst->set_filename( "prefix####.jpg" );
    pFirst->set_recin( 10 );
    pFirst->set_recout( 13 );
    pFirst->set_srcin( 10 );
    ::protocol::duke::Clip* const pSecond = playlist.add_clip();
    pSecond->set_path( "../toto" );
    pSecond->set_filename( "prefix_####_suffix.dpx" );
    pSecond->set_recin( 11 );
    pSecond->set_recout( 14 );
    return playlist;
}

BOOST_AUTO_TEST_CASE( TestAccelerators )
{
    typedef PlaylistHelper::FrameToIndices FrameToIndices;

    PlaylistHelper accelerator(buildOneFramePlaylist());
    BOOST_CHECK_EQUAL( accelerator.getFrameCount(), 1u);
    BOOST_CHECK_EQUAL( accelerator.getEndIterator(), 1u);
    BOOST_CHECK_EQUAL( accelerator.getRecIn(), 10u);
    BOOST_CHECK_EQUAL( accelerator.getRecOut(), 11u);
    BOOST_REQUIRE_EQUAL( accelerator.m_mFrameToClipIndices.size(), 2u);
    FrameToIndices::const_iterator it = accelerator.m_mFrameToClipIndices.begin();
    BOOST_CHECK_EQUAL( accelerator.getFrame(it), 10u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it).size(), 1u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[0], 0u);
    ++it;
    BOOST_CHECK_EQUAL( accelerator.getIndices(it).size(), 0u);
}

/**
 * representing the following playlist
 *
 * 2      # #
 * 1    # # # #
 * 0  # # #
 *   10  12  14  16
 *     11  13  15
 *
 * frame 10 to 12 track 0 (rec:10-13)
 * frame 11 to 14 track 1 (rec:11-15)
 * frame 12 to 13 track 2 (rec:12-14)
 */
::protocol::duke::Playlist buildTrickyPlaylist()
{
    ::protocol::duke::Playlist playlist;
    ::protocol::duke::Clip* pClip = playlist.add_clip();
    pClip->set_filename( "_0_##" );
    pClip->set_recin( 10 );
    pClip->set_recout( 13 );
    pClip = playlist.add_clip();
    pClip->set_filename( "_1_##" );
    pClip->set_recin( 11 );
    pClip->set_recout( 15 );
    pClip = playlist.add_clip();
    pClip->set_filename( "_2_##" );
    pClip->set_recin( 12 );
    pClip->set_recout( 14 );
    return playlist;
}

BOOST_AUTO_TEST_CASE( accelerator_frame_structure )
{
    typedef PlaylistHelper::FrameToIndices FrameToIndices;
    PlaylistHelper accelerator(buildTrickyPlaylist());
    BOOST_CHECK_EQUAL( accelerator.getFrameCount(), 5u);
    BOOST_CHECK_EQUAL( accelerator.getEndIterator(), 9u);
    BOOST_CHECK_EQUAL( accelerator.getRecIn(), 10u);
    BOOST_CHECK_EQUAL( accelerator.getRecOut(), 15u);
    BOOST_CHECK_EQUAL( accelerator.m_mFrameToClipIndices.size(), 6u);
    FrameToIndices::const_iterator it = accelerator.m_mFrameToClipIndices.begin();
    BOOST_CHECK_EQUAL( accelerator.getFrame(it), 10u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it).size(), 1u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[0], 0u);
    ++it;
    BOOST_CHECK_EQUAL( accelerator.getFrame(it), 11u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it).size(), 2u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[0], 0u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[1], 1u);
    ++it;
    BOOST_CHECK_EQUAL( accelerator.getFrame(it), 12u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it).size(), 3u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[0], 0u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[1], 1u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[2], 2u);
    ++it;
    BOOST_CHECK_EQUAL( accelerator.getFrame(it), 13u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it).size(), 2u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[0], 1u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[1], 2u);
    ++it;
    BOOST_CHECK_EQUAL( accelerator.getFrame(it), 14u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it).size(), 1u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it)[0], 1u);
    ++it;
    BOOST_CHECK_EQUAL( accelerator.getFrame(it), 15u);
    BOOST_CHECK_EQUAL( accelerator.getIndices(it).size(), 0u);
}

BOOST_AUTO_TEST_CASE( iterators )
{
    PlaylistHelper accelerator(buildTrickyPlaylist());
    BOOST_CHECK_EQUAL( accelerator.getPathAtIterator(0).string(), "_0_00");
    BOOST_CHECK_EQUAL( accelerator.getPathAtIterator(7).string(), "_2_01");
}

void check(
        const PlaylistHelper &accelerator, //
        const PlaylistHelper::IteratorToFrame::const_iterator& it, //
        const size_t itrValue) {
    BOOST_CHECK_EQUAL( it->first, itrValue);
    BOOST_CHECK_EQUAL( accelerator.getItr(it->second), itrValue);
}

BOOST_AUTO_TEST_CASE( accelerator_iterator )
{

    PlaylistHelper accelerator(buildTrickyPlaylist());
    // checking the iterator accelerator
    typedef PlaylistHelper::IteratorToFrame IteratorToFrame;
    IteratorToFrame::const_iterator it = accelerator.m_mIteratorToFrame.begin();
    check(accelerator, it++,0u);
    check(accelerator, it++,1u);
    check(accelerator, it++,3u);
    check(accelerator, it++,6u);
    check(accelerator, it++,8u);
    check(accelerator, it++,9u);
    BOOST_CHECK( it == accelerator.m_mIteratorToFrame.end() );
}

BOOST_AUTO_TEST_CASE( accelerator_iterator2 )
{
    typedef PlaylistHelper::FrameToIndices FrameToIndices;
    PlaylistHelper accelerator(buildTrickyPlaylist());
    BOOST_CHECK_EQUAL( accelerator.getIteratorIndexAtFrame(0), accelerator.getEndIterator() );
    BOOST_CHECK_EQUAL( accelerator.getIteratorIndexAtFrame(10), 0u );
    BOOST_CHECK_EQUAL( accelerator.getIteratorIndexAtFrame(14), 8u );
    BOOST_CHECK_EQUAL( accelerator.getIteratorIndexAtFrame(15), accelerator.getEndIterator() );
}

BOOST_AUTO_TEST_CASE( accelerators_iterators_at_frame )
{
    std::vector<size_t> indices;

    typedef PlaylistHelper::FrameToIndices FrameToIndices;
    PlaylistHelper accelerator(buildTrickyPlaylist());

    accelerator.getIteratorsAtFrame(0, indices);
    BOOST_CHECK( indices.empty() );
    accelerator.getIteratorsAtFrame(10, indices);
    BOOST_CHECK_EQUAL( indices.size(), 1u );
    BOOST_CHECK_EQUAL( indices[0], 0u );
    accelerator.getIteratorsAtFrame(12, indices);
    BOOST_CHECK_EQUAL( indices.size(), 3u );
    BOOST_CHECK_EQUAL( indices[0], 3u );
    BOOST_CHECK_EQUAL( indices[1], 4u );
    BOOST_CHECK_EQUAL( indices[2], 5u );
    accelerator.getIteratorsAtFrame(14, indices);
    BOOST_CHECK_EQUAL( indices.size(), 1u );
    BOOST_CHECK_EQUAL( indices[0], 8u );
    accelerator.getIteratorsAtFrame(15, indices);
    BOOST_CHECK( indices.empty() );
}

BOOST_AUTO_TEST_SUITE_END()
