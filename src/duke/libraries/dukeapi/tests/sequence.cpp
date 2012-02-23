#include <player.pb.h>
#include <dukeapi/sequence/PlaylistBuilder.h>
#include <dukeapi/sequence/PlaylistHelper.h>

#include <stdexcept>

#define BOOST_TEST_MODULE DukeApiSequence
#include <boost/test/unit_test.hpp>

using namespace duke::protocol;
using namespace sequence;
using namespace std;

BOOST_AUTO_TEST_SUITE( SequenceTestSuite )

BOOST_AUTO_TEST_CASE( overlappingRanges ) {
    PlaylistBuilder builder;
    TrackBuilder tb = builder.addTrack("track01");
    tb.addImage("image.jpg", Range(0,0));
    tb.addImage("image.jpg", Range(0,0));
    BOOST_CHECK_THROW( PlaylistHelper h(builder), runtime_error);
}

BOOST_AUTO_TEST_CASE( trackHelperRecordTest ) {
    {
        PlaylistBuilder builder;
        BOOST_CHECK_EQUAL( 0U, builder.addTrack("t").currentRecord() );
        BOOST_CHECK_EQUAL( 10U, builder.addTrack("t2",10).currentRecord() );
    }
    {
        PlaylistBuilder builder;
        TrackBuilder tb = builder.addTrack("t");
        BOOST_CHECK_EQUAL( 0U, tb.currentRecord() );
        BrowseItem file = sequence::create_file("filename");
        const Media &fileMedia = tb.addBrowseItem(file);
        BOOST_CHECK_EQUAL( Media_Type_SINGLE_IMAGE, fileMedia.type() );
        BOOST_CHECK( !fileMedia.has_source() );
        BOOST_CHECK_EQUAL( 1U, tb.currentRecord() );
        SequencePattern pattern("file-",".jpg",4);
        Range src(10,20); // 11 images
        BrowseItem sequence = sequence::create_sequence("/root", pattern, src,1);
        const Media &sequenceMedia = tb.addBrowseItem(sequence);
        BOOST_CHECK_EQUAL( Media_Type_IMAGE_SEQUENCE, sequenceMedia.type() );
        BOOST_CHECK( sequenceMedia.has_source() );
        BOOST_CHECK_EQUAL( 10U, sequenceMedia.source().first() );
        BOOST_CHECK_EQUAL( 20U, sequenceMedia.source().last() );
        BOOST_CHECK_EQUAL( 1U+11U, tb.currentRecord() );
    }
}

BOOST_AUTO_TEST_CASE( playlistHelper ) {
    PlaylistBuilder builder;
    {
        TrackBuilder tb = builder.addTrack("track01");
        tb.addImage("image.jpg", Range(0,9));
        tb.addImage("image.jpg", Range(10,19));
    }
    {
        TrackBuilder tb = builder.addTrack("track02");
        tb.addSequence("file####.jpg", Range(1,20), Range(0,5));
    }

    const PlaylistHelper helper(builder);
    BOOST_CHECK_EQUAL( 0U, helper.range.first );
    BOOST_CHECK_EQUAL( 20U, helper.range.last );

    BOOST_CHECK_EQUAL( 2u, helper.tracks.size() );

    BOOST_CHECK_EQUAL( 0u, helper.tracks[0].range.first );
    BOOST_CHECK_EQUAL( 19u, helper.tracks[0].range.last );
    BOOST_CHECK_EQUAL( 1u, helper.tracks[1].range.first );
    BOOST_CHECK_EQUAL( 20u, helper.tracks[1].range.last );
}

BOOST_AUTO_TEST_SUITE_END()
