#include <gtest/gtest.h>

#include <duke/timeline/Timeline.h>

#include <stdexcept>

using namespace std;

TEST(Track,findClip) {
	Track track;
	auto &clips = track.clips;
	EXPECT_EQ(MediaFrameReference(), track.clipAt(0));
	track.add(0, Clip { 1 });
	const auto pFirst = &(clips.begin())->second;
	EXPECT_EQ(MediaFrameReference(pFirst,0), track.clipAt(0));
	EXPECT_EQ(MediaFrameReference(), track.clipAt(1));
	track.add(10, Clip { 1 });
	const auto pLast = &(clips.rbegin())->second;
	EXPECT_EQ(MediaFrameReference(), track.clipAt(9));
	EXPECT_EQ(MediaFrameReference(pLast,0), track.clipAt(10));
	EXPECT_EQ(MediaFrameReference(), track.clipAt(11));
	track.add(5, Clip { 3 });
	const auto pMiddle = &(++clips.begin())->second;
	EXPECT_EQ(MediaFrameReference(), track.clipAt(4));
	EXPECT_EQ(MediaFrameReference(pMiddle,0), track.clipAt(5));
	EXPECT_EQ(MediaFrameReference(pMiddle,1), track.clipAt(6));
	EXPECT_EQ(MediaFrameReference(pMiddle,2), track.clipAt(7));
	EXPECT_EQ(MediaFrameReference(), track.clipAt(8));
}

TEST(Track,range) {
	Track track;
	EXPECT_THROW(track.getRange(), std::runtime_error);
	track.add(1, Clip { 1 });
	EXPECT_EQ(Range(1,1), track.getRange());
	track.add(10, Clip { 10 });
	EXPECT_EQ(Range(1,19), track.getRange());
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
	track1.clips.insert(make_pair(1, Clip { 1 }));
	track1.clips.insert(make_pair(3, Clip { 1 }));
	Track track2;
	track2.clips.insert(make_pair(0, Clip { 5 }));
	Track track3;
	Timeline timeline;
	timeline.startFrame = 4;
	timeline.tracks.push_back(track1);
	timeline.tracks.push_back(track2);
	timeline.tracks.push_back(track3);
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

TEST(Timeline,range) {
	Timeline timeline;
	auto &tracks = timeline.tracks;
	EXPECT_EQ(Range::EMPTY, timeline.getRange());
	//no tracks
	tracks.emplace_back();
	EXPECT_EQ(Range::EMPTY, timeline.getRange());
	// empty tracks
	tracks[0].add(10, Clip { 1 });
	EXPECT_EQ(Range(10,10), timeline.getRange());
	timeline.startFrame = 10;
	EXPECT_EQ(Range(20,20), timeline.getRange());
	tracks.emplace_back();
	tracks[1].add(15, Clip { 6 });
	EXPECT_EQ(Range(20,30), timeline.getRange());
}
