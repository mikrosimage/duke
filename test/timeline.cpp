#include <gtest/gtest.h>

#include <duke/engine/Timeline.hpp>
#include <duke/engine/streams/IMediaStream.hpp>

#include <stdexcept>
#include <set>

using namespace std;
using namespace duke;

class DummyMediaStream: public IMediaStream {
public:
	virtual void generateFilePath(string &path, size_t atFrame) const {
	}
};

TEST(Track,findClip) {
	auto pStream1 = make_shared<DummyMediaStream>();
	auto pStream2 = make_shared<DummyMediaStream>();
	auto pStream3 = make_shared<DummyMediaStream>();
	Track track;
	EXPECT_EQ(MediaFrameReference(), track.getMediaFrameReferenceAt(0));
	track.add(0, Clip { 1, pStream1 });
	EXPECT_EQ(MediaFrameReference(pStream1.get(),0), track.getMediaFrameReferenceAt(0));
	EXPECT_EQ(MediaFrameReference(), track.getMediaFrameReferenceAt(1));
	track.add(10, Clip { 1, pStream2 });
	EXPECT_EQ(MediaFrameReference(), track.getMediaFrameReferenceAt(9));
	EXPECT_EQ(MediaFrameReference(pStream2.get(),0), track.getMediaFrameReferenceAt(10));
	EXPECT_EQ(MediaFrameReference(), track.getMediaFrameReferenceAt(11));
	track.add(5, Clip { 3, pStream3 });
	EXPECT_EQ(MediaFrameReference(), track.getMediaFrameReferenceAt(4));
	EXPECT_EQ(MediaFrameReference(pStream3.get(),0), track.getMediaFrameReferenceAt(5));
	EXPECT_EQ(MediaFrameReference(pStream3.get(),1), track.getMediaFrameReferenceAt(6));
	EXPECT_EQ(MediaFrameReference(pStream3.get(),2), track.getMediaFrameReferenceAt(7));
	EXPECT_EQ(MediaFrameReference(), track.getMediaFrameReferenceAt(8));
}

TEST(Track,range) {
	Track track;
	EXPECT_THROW(track.getRange(), std::runtime_error);
	track.add(1, Clip { 1 });
	EXPECT_EQ(Range(1,1), track.getRange());
	track.add(10, Clip { 10 });
	EXPECT_EQ(Range(1,19), track.getRange());
}

TEST(Track,findclips) {
	Track track;
	EXPECT_EQ(track.end(), track.clipContaining(1));
	EXPECT_EQ(track.end(), track.nextClip(1));
	EXPECT_EQ(track.end(), track.previousClip(1));

	track.add(1, Clip { 1 });
	EXPECT_EQ(track.begin(), track.clipContaining(1));
	EXPECT_EQ(track.end(), track.nextClip(1));
	EXPECT_EQ(track.begin(), track.nextClip(0));
	EXPECT_EQ(track.end(), track.previousClip(1));
	EXPECT_EQ(track.begin(), track.previousClip(2));

	track.add(5, Clip { 5 });
	const auto pLast = track.find(5);
	EXPECT_EQ(pLast, track.previousClip(10));
	EXPECT_EQ(track.begin(), track.previousClip(9));
	EXPECT_EQ(pLast, track.clipContaining(9));
	EXPECT_EQ(pLast, track.clipContaining(5));
	EXPECT_EQ(pLast, track.nextClip(2));
}

/**
 * Three tracks
 *  1| * *
 *  2|*****
 *  3|
 *  ---------
 *    456789
 *    ^
 *   timeline start : 4
 */

TEST(Timeline,findClips) {
//setup
	Track track1;
	track1.insert(make_pair(5, Clip { 1, make_shared<DummyMediaStream>() }));
	track1.insert(make_pair(7, Clip { 1, make_shared<DummyMediaStream>() }));
	Track track2;
	track2.insert(make_pair(4, Clip { 5, make_shared<DummyMediaStream>() }));
	Track track3;
	Timeline timeline;
	timeline.push_back(track1);
	timeline.push_back(track2);
	timeline.push_back(track3);
// test
	vector<MediaFrameReference> refs;
	auto check = [&](int frameA, int frameB, int frameC) {
		ASSERT_EQ(3, refs.size());
		auto checkOne = [](int frame, MediaFrameReference actual) {
			if(frame<0) {
				EXPECT_EQ(MediaFrameReference(), actual);
			} else {
				EXPECT_EQ(frame, actual.second);
			}
		};
		checkOne(frameA, refs[0]);
		checkOne(frameB, refs[1]);
		checkOne(frameC, refs[2]);
	};
	timeline.populateMediaAt(4, refs);
	check(-1, 0, -1);
	timeline.populateMediaAt(5, refs);
	check(0, 1, -1);
	timeline.populateMediaAt(6, refs);
	check(-1, 2, -1);
	timeline.populateMediaAt(7, refs);
	check(0, 3, -1);
	timeline.populateMediaAt(8, refs);
	check(-1, 4, -1);
	timeline.populateMediaAt(9, refs);
	check(-1, -1, -1);
}

TEST(Timeline,empty) {
	EXPECT_TRUE(Timeline().empty());
	Timeline t;
	t.emplace_back();
	t.emplace_back();
	EXPECT_TRUE(t.empty());
	t.front().add(0, Clip { 1 });
}

TEST(Timeline,range) {
	Timeline timeline;
	EXPECT_EQ(Range::EMPTY, timeline.getRange());
	//no tracks
	timeline.emplace_back();
	EXPECT_EQ(Range::EMPTY, timeline.getRange());
	// empty tracks
	timeline[0].add(10, Clip { 1 });
	EXPECT_EQ(Range(10,10), timeline.getRange());
	timeline.emplace_back();
	timeline[1].add(15, Clip { 6 });
	EXPECT_EQ(Range(10,20), timeline.getRange());
}

TEST(Timeline, skip) {
	set<std::size_t> s;
	EXPECT_EQ(s.end(), findLess(s,1));
	EXPECT_EQ(s.end(), findLessOrEquals(s,1));

	s.insert(5);
	EXPECT_EQ(s.end(), findLess(s,1));
	EXPECT_EQ(s.end(), findLess(s,5));
	EXPECT_EQ(s.begin(), findLess(s,6));
	EXPECT_EQ(s.end(), findLessOrEquals(s,1));
	EXPECT_EQ(s.begin(), findLessOrEquals(s,5));
	EXPECT_EQ(s.begin(), findLessOrEquals(s,6));
}
