#include <player.pb.h>
#include <dukeapi/protobuf_builder/PlaylistBuilder.h>
#include <dukeapi/sequence/PlaylistHelper.h>

#include <stdexcept>

#define BOOST_TEST_MODULE DukeApiSequence
#include <boost/test/unit_test.hpp>

using namespace duke::protocol;
using namespace sequence;
using namespace std;

static inline void check(const Range &a, const Range &b) {
    BOOST_CHECK_EQUAL(a.first, b.first);
    BOOST_CHECK_EQUAL(a.last, b.last);
}

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
        const Media &fileMedia = tb.addBrowseItem(file).media();
        BOOST_CHECK_EQUAL( Media_Type_SINGLE_IMAGE, fileMedia.type() );
        BOOST_CHECK( !fileMedia.has_source() );
        BOOST_CHECK_EQUAL( 1U, tb.currentRecord() );
        SequencePattern pattern("file-",".jpg",4);
        Range src(10,20); // 11 images
        BrowseItem sequence = sequence::create_sequence("/root", pattern, src,1);
        const Media &sequenceMedia = tb.addBrowseItem(sequence).media();
        BOOST_CHECK_EQUAL( Media_Type_IMAGE_SEQUENCE, sequenceMedia.type() );
        BOOST_CHECK( sequenceMedia.has_source() );
        BOOST_CHECK_EQUAL( 10U, sequenceMedia.source().first() );
        BOOST_CHECK_EQUAL( 20U, sequenceMedia.source().last() );
        BOOST_CHECK_EQUAL( 1U+11U, tb.currentRecord() );
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( PlaylistHelperTestSuite )

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
    check( helper.range, Range(0,20));

    BOOST_CHECK_EQUAL( helper.tracks.size(), 2u);

    check( helper.tracks[0].range, Range(0,19));
    check( helper.tracks[1].range, Range(1,20));

    BOOST_CHECK_EQUAL( helper.allClips.size(), 3 );
    check( helper.allClips[0], Range(0,9));
    check( helper.allClips[1], Range(1,20));
    check( helper.allClips[2], Range(10,19));
}

BOOST_AUTO_TEST_SUITE_END()
