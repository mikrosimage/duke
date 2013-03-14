#include <gtest/gtest.h>

#include <duke/engine/cache/TimelineIterator.h>
#include <duke/engine/Timeline.h>
#include <duke/engine/streams/IMediaStream.h>

#include <stdexcept>
#include <set>

using namespace std;
using namespace duke;

ostream& operator<<(ostream& stream, const Range &range) {
	return stream << '[' << range.first << ',' << range.last << ']';
}

class DummyMediaStream: public IMediaStream {
public:
	virtual void generateFilePath(string &path, size_t atFrame) const {
	}
};

static shared_ptr<IMediaStream> pStream = make_shared<DummyMediaStream>();

TEST(TrackMediaFrameIterator, emptiness) {
	EXPECT_TRUE(TrackMediaFrameIterator().empty());
	EXPECT_TRUE(TrackMediaFrameIterator(nullptr,0UL).empty());
}

TEST(TrackMediaFrameIterator, oneFrame) {
	Timeline timeline = { Track() };
	Track &track = timeline.back();
	track.add(0, Clip { 1, pStream });
	TrackMediaFrameIterator itr(&timeline, 0UL);
	EXPECT_FALSE(itr.empty());
	EXPECT_EQ(MediaFrameReference(pStream.get(), 0UL), itr.next());
	EXPECT_TRUE(itr.empty());
}

TEST(TrackMediaFrameIterator, clear) {
	Timeline timeline = { Track() };
	Track &track = timeline.back();
	track.add(0, Clip { 1, pStream });
	TrackMediaFrameIterator itr(&timeline, 0UL);
	EXPECT_FALSE(itr.empty());
	itr.clear();
	EXPECT_TRUE(itr.empty());
}

TEST(TrackMediaFrameIterator, outsideRange) {
	Timeline timeline = { Track() };
	Track &track = timeline.back();
	track.add(0, Clip { 1, pStream });
	EXPECT_TRUE(TrackMediaFrameIterator(&timeline, 1UL).empty());
}

TEST(TrackMediaFrameIterator, noMedia) {
	Timeline timeline = { Track() };
	Track &track = timeline.back();
	track.add(0, Clip { 1, nullptr });
	EXPECT_TRUE(TrackMediaFrameIterator(&timeline, 0UL).empty());
}

TEST(FrameIterator, emptiness) {
	EXPECT_TRUE(FrameIterator(nullptr,0UL).empty());
	Ranges ranges;
	EXPECT_TRUE(FrameIterator(&ranges,0UL).empty());
}

static void checkIteration(FrameIterator &&itr, const vector<size_t> &frames) {
	for (size_t frame : frames) {
		ASSERT_FALSE(itr.empty());
		EXPECT_EQ(frame, itr.next());
	}
	EXPECT_TRUE(itr.empty());
}

/**
 *
 *  [##___#__#]
 *   012345678
 *      ^
 *
 *  timeline with 4 real frames
 *  start frame in a hole
 */
TEST(FrameIterator, allDirectionsWithHoles) {
	Ranges ranges = { Range(0, 1), Range(5, 5), Range(8, 8) };
	size_t startFrame = 3;
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::FORWARD), { 5, 8, 0, 1 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::BACKWARD), { 1, 0, 8, 5 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::PINGPONG), { 5, 1, 8, 0 });
	startFrame = 1;
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::FORWARD), { 1, 5, 8, 0 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::BACKWARD), { 1, 0, 8, 5 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::PINGPONG), { 1, 5, 0, 8 });
}

TEST(FrameIterator, allDirectionsNoHole) {
	Ranges ranges = { Range(0, 3) };
	size_t startFrame = 0;
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::FORWARD), { 0, 1, 2, 3 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::BACKWARD), { 0, 3, 2, 1 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::PINGPONG), { 0, 1, 3, 2 });
	startFrame = 1;
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::FORWARD), { 1, 2, 3, 0 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::BACKWARD), { 1, 0, 3, 2 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::PINGPONG), { 1, 2, 0, 3 });
	startFrame = 3;
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::FORWARD), { 3, 0, 1, 2 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::BACKWARD), { 3, 2, 1, 0 });
	checkIteration(FrameIterator(&ranges, startFrame, IterationMode::PINGPONG), { 3, 0, 2, 1 });
}

TEST(TimelineIterator, emptiness) {
	EXPECT_TRUE(TimelineIterator().empty());
	Timeline timeline;
	Ranges mediaRanges = getMediaRanges(timeline);
	EXPECT_TRUE(TimelineIterator(&timeline, &mediaRanges,0).empty());
	timeline.emplace_back(); // adding an empty track
	mediaRanges = getMediaRanges(timeline);
	EXPECT_TRUE(TimelineIterator(&timeline, &mediaRanges,0).empty());
}

TEST(TimelineIterator, oneFrameStartingFromTheFrame) {
	Timeline timeline = { Track() };
	Track &track = timeline.back();
	track.add(0, Clip { 1, pStream });
	const Ranges mediaRanges = getMediaRanges(timeline);
	TimelineIterator itr(&timeline, &mediaRanges, 0);
	EXPECT_FALSE(itr.empty());
	EXPECT_EQ(MediaFrameReference(pStream.get(),0), itr.next());
	EXPECT_TRUE(itr.empty());
}

TEST(TimelineIterator, oneFrameStartingFromElsewhere) {
	Timeline timeline = { Track() };
	Track &track = timeline.back();
	track.add(0, Clip { 1, pStream });
	const Ranges mediaRanges = getMediaRanges(timeline);
	EXPECT_FALSE(mediaRanges.empty());
	TimelineIterator itr(&timeline, &mediaRanges, 100);
	EXPECT_FALSE(itr.empty());
	EXPECT_EQ(MediaFrameReference(pStream.get(),0), itr.next());
	EXPECT_TRUE(itr.empty());
}

/**
 * Five frames and three tracks
 *    0 1 2 3 4
 *  0 X       X
 *  1     X X X
 *  2       X X
 */TEST(TimelineIterator, complexTimeline) {
	Timeline timeline { Track(), Track(), Track() };
	Track &track1 = timeline[0];
	Track &track2 = timeline[1];
	Track &track3 = timeline[2];
	track1.add(0, Clip { 1, make_shared<DummyMediaStream>() });
	track1.add(4, Clip { 1, make_shared<DummyMediaStream>() });
	track2.add(2, Clip { 3, make_shared<DummyMediaStream>() });
	track3.add(3, Clip { 2, make_shared<DummyMediaStream>() });
	const IMediaStream *pStream1 = track1.begin()->second.pStream.get();
	const IMediaStream *pStream2 = track2.begin()->second.pStream.get();
	const IMediaStream *pStream3 = track3.begin()->second.pStream.get();
	const IMediaStream *pStream4 = track1.rbegin()->second.pStream.get();
	const Ranges mediaRange = getMediaRanges(timeline);
	{
		TimelineIterator itr(&timeline, &mediaRange, 0);
		EXPECT_FALSE(itr.empty());
		EXPECT_EQ(MediaFrameReference(pStream1,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream2,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream2,1UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream3,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream4,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream2,2UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream3,1UL), itr.next());
		EXPECT_TRUE(itr.empty());
	}
	{
		TimelineIterator itr(&timeline, &mediaRange, 1);
		EXPECT_FALSE(itr.empty());
		EXPECT_EQ(MediaFrameReference(pStream2,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream2,1UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream3,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream4,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream2,2UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream3,1UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pStream1,0UL), itr.next());
		EXPECT_TRUE(itr.empty());
	}
}

TEST(TimelineMediaRange, empty) {
	EXPECT_TRUE(getMediaRanges(Timeline()).empty());
}

TEST(TimelineMediaRange, oneTrack) {
	Timeline timeline = { Track(), Track() };
	Track &frontTrack = timeline.front();
	frontTrack.add(0, Clip { 1 });
	EXPECT_TRUE(getMediaRanges(timeline).empty()) << "no stream attached";
	frontTrack.begin()->second.pStream = pStream;
	{
		const Ranges ranges = getMediaRanges(timeline);
		EXPECT_EQ(1UL, ranges.size());
		EXPECT_EQ(Range(0,0), ranges[0]);
	}
	frontTrack.add(2, Clip { 1, pStream });
	{
		const Ranges ranges = getMediaRanges(timeline);
		EXPECT_EQ(2UL, ranges.size());
		EXPECT_EQ(Range(0,0), ranges[0]);
		EXPECT_EQ(Range(2,2), ranges[1]);
	}
	Track &backTrack = timeline.back();
	backTrack.add(0, Clip { 3, pStream });
	{
		const Ranges ranges = getMediaRanges(timeline);
		EXPECT_EQ(1UL, ranges.size());
		EXPECT_EQ(Range(0,2), ranges[0]);
	}
}

TEST(TimelineMediaRange, contains) {
	EXPECT_TRUE(contains(Ranges {Range(0,0)}, 0));
	EXPECT_FALSE(contains(Ranges {Range(0,0)}, 1));
	Ranges ranges { Range(0, 0), Range(2, 2) };
	EXPECT_TRUE(contains(ranges, 0));
	EXPECT_FALSE(contains(ranges, 1));
	EXPECT_TRUE(contains(ranges, 2));
	EXPECT_FALSE(contains(ranges, 3));
}
