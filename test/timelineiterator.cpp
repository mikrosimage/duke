#include <gtest/gtest.h>

#include <duke/engine/imagecache/TimelineIterator.h>
#include <duke/engine/Timeline.h>
#include <duke/engine/streams/IMediaStream.h>

#include <stdexcept>
#include <set>

using namespace std;
using namespace duke;

std::ostream& operator<<(std::ostream& stream, const Range &range) {
	return stream << '[' << range.first << ',' << range.last << ']';
}

class DummyMediaStream: public IMediaStream {
public:
	virtual void generateFilePath(std::string &path, size_t atFrame) const {
	}
};

static std::shared_ptr<IMediaStream> pStream = std::make_shared<DummyMediaStream>();

TEST(TimelineIterator, emptyness) {
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
	Clip &clip = track.begin()->second;
	const Ranges mediaRanges = getMediaRanges(timeline);
	TimelineIterator itr(&timeline, &mediaRanges, 0);
	EXPECT_FALSE(itr.empty());
	EXPECT_EQ(MediaFrameReference(&clip,0), itr.next());
	EXPECT_TRUE(itr.empty());
}

TEST(TimelineIterator, oneFrameStartingFromElsewhere) {
	Timeline timeline = { Track() };
	Track &track = timeline.back();
	track.add(0, Clip { 1, pStream });
	Clip &clip = track.begin()->second;
	const Ranges mediaRanges = getMediaRanges(timeline);
	EXPECT_FALSE(mediaRanges.empty());
	TimelineIterator itr(&timeline, &mediaRanges, 100);
	EXPECT_FALSE(itr.empty());
	EXPECT_EQ(MediaFrameReference(&clip,0), itr.next());
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
	track1.add(0, Clip { 1, pStream });
	track1.add(4, Clip { 1, pStream });
	track2.add(2, Clip { 3, pStream });
	track3.add(3, Clip { 2, pStream });
	const Clip *pClip1 = &track1.begin()->second;
	const Clip *pClip2 = &track2.begin()->second;
	const Clip *pClip3 = &track3.begin()->second;
	const Clip *pClip4 = &track1.rbegin()->second;
	const Ranges mediaRange = getMediaRanges(timeline);
	{
		TimelineIterator itr(&timeline, &mediaRange, 0);
		EXPECT_FALSE(itr.empty());
		EXPECT_EQ(MediaFrameReference(pClip1,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip2,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip2,1UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip3,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip4,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip2,2UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip3,1UL), itr.next());
		EXPECT_TRUE(itr.empty());
	}
	{
		TimelineIterator itr(&timeline, &mediaRange, 1);
		EXPECT_FALSE(itr.empty());
		EXPECT_EQ(MediaFrameReference(pClip2,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip2,1UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip3,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip4,0UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip2,2UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip3,1UL), itr.next());
		EXPECT_EQ(MediaFrameReference(pClip1,0UL), itr.next());
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
