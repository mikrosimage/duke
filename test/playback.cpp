#include <gtest/gtest.h>

#include <duke/engine/Player.h>

static const size_t startFrame = 90000;

using namespace duke;

Timeline getTimeline() {
	Track track;
	track.add(startFrame, Clip { 1 });
	track.add(startFrame + 1, Clip { 1 });
	track.add(startFrame + 2, Clip { 1 });
	track.add(startFrame + 3, Clip { 1 });
	return {track};
}

TEST(Player,time) {
	Player player;
	EXPECT_EQ(FrameIndex(), player.getCurrentFrame());
	player.setPlaybackTime(10); // go to ten seconds
	EXPECT_EQ(FrameIndex(25*10), player.getCurrentFrame());
	player.load(getTimeline(), FrameDuration::PAL);
	// should be at the beginning of the timeline
	EXPECT_EQ(Time(3600), player.getPlaybackTime());
	EXPECT_EQ(FrameIndex(startFrame ), player.getCurrentFrame());
	player.setPlaybackTime(3610); // go to ten seconds
	EXPECT_EQ(FrameIndex(startFrame + 10*25), player.getCurrentFrame());
}

TEST(Player,playback) {
	Player player;
	player.load(getTimeline(), FrameDuration::PAL);
	player.setPlaybackMode(Player::CONTINUE);
	// playing
	player.setPlaybackSpeed(1);
	player.offsetPlaybackTime(player.getFrameDuration());
	const auto currentFrame = player.getCurrentFrame();
	EXPECT_EQ(FrameIndex(startFrame + 1), currentFrame);

	// pausing
	player.setPlaybackSpeed(0);
	player.offsetPlaybackTime(player.getFrameDuration());
	EXPECT_EQ(currentFrame, player.getCurrentFrame());

	// fast forward
	player.setPlaybackSpeed(10);
	player.offsetPlaybackTime(player.getFrameDuration());
	EXPECT_EQ(currentFrame+10, player.getCurrentFrame());

	// fast backward
	player.setPlaybackSpeed(-10);
	player.offsetPlaybackTime(player.getFrameDuration());
	EXPECT_EQ(currentFrame, player.getCurrentFrame());
}

TEST(Player,stopping) {
	Player player;
	player.setPlaybackMode(Player::STOP);
	const auto timeline = getTimeline();
	const auto range = timeline.getRange();
	player.load(timeline, FrameDuration::PAL);
	EXPECT_EQ(range.first, player.getCurrentFrame().round());
	EXPECT_EQ(Player::STOP, player.getPlaybackMode());
	player.setPlaybackSpeed(-1);
	player.offsetPlaybackTime(player.getFrameDuration()); // should stop at first frame
	EXPECT_EQ(0, player.getPlaybackSpeed());
	EXPECT_EQ(range.first, player.getCurrentFrame().round());
	player.cue(range.last);
	player.setPlaybackSpeed(1);
	player.offsetPlaybackTime(player.getFrameDuration()); // should stop at last frame
	EXPECT_EQ(0, player.getPlaybackSpeed());
	EXPECT_EQ(range.last, player.getCurrentFrame().round());
}

TEST(Player,looping) {
	Player player;
	const auto timeline = getTimeline();
	const auto range = timeline.getRange();
	player.load(timeline, FrameDuration::PAL);
	EXPECT_EQ(range.first, player.getCurrentFrame().round());
	EXPECT_EQ(Player::LOOP, player.getPlaybackMode());
	// moving one frame
	player.setPlaybackSpeed(-1);
	player.offsetPlaybackTime(player.getFrameDuration()); // reverse should go last frame
	EXPECT_EQ(range.last, player.getCurrentFrame().round());
	player.setPlaybackSpeed(1);
	player.offsetPlaybackTime(player.getFrameDuration()); // forward should go first frame
	EXPECT_EQ(range.first, player.getCurrentFrame().round());
	// moving two frames
	player.setPlaybackSpeed(-2);
	player.offsetPlaybackTime(player.getFrameDuration()); // reverse should go last frame
	EXPECT_EQ(range.last-1, player.getCurrentFrame().round());
	player.setPlaybackSpeed(2);
	player.offsetPlaybackTime(player.getFrameDuration()); // forward should go first frame
	EXPECT_EQ(range.first, player.getCurrentFrame().round());
}

TEST(Player,looping2) {
	Player player;
	Track track;
	track.add(0, Clip { 1 });
	track.add(1, Clip { 1 });
	track.add(2, Clip { 1 });
	track.add(3, Clip { 1 });
	const auto timeline = Timeline { track };
	const auto range = timeline.getRange();
	auto advance = [&]() {player.offsetPlaybackTime(player.getFrameDuration());};
	EXPECT_EQ(range, Range (0,3));
	player.load(timeline, FrameDuration::PAL);
	EXPECT_EQ(Player::LOOP, player.getPlaybackMode());
	// moving one frame
	player.setPlaybackSpeed(-1);
	advance(); // reverse should go last frame
	EXPECT_EQ(FrameIndex(3), player.getCurrentFrame());
	player.setPlaybackSpeed(1);
	advance(); // forward should go first frame
	EXPECT_EQ(FrameIndex(0), player.getCurrentFrame());
	advance();
	EXPECT_EQ(FrameIndex(1), player.getCurrentFrame());
	advance();
	EXPECT_EQ(FrameIndex(2), player.getCurrentFrame());
	advance();
	EXPECT_EQ(FrameIndex(3), player.getCurrentFrame());
	advance();
	EXPECT_EQ(FrameIndex(0), player.getCurrentFrame());
}

TEST(Player,loopingWithHugeStep) {
	// if in looping mode with offset greater than timeline period, just doing nothing
	Player player;
	const auto timeline = getTimeline();
	const auto range = timeline.getRange();
	player.load(timeline, FrameDuration::PAL);
	EXPECT_EQ(range.first, player.getCurrentFrame().round());
	EXPECT_EQ(Player::LOOP, player.getPlaybackMode());
	player.setPlaybackSpeed(1);
	const auto lotsOfFrames = (range.last - range.first + 1) * 5 / 2;
	player.offsetPlaybackTime(player.getFrameDuration() * lotsOfFrames);
	EXPECT_EQ(range.first, player.getCurrentFrame().round());
}
