#include <gtest/gtest.h>

#include <duke/cmdline/CmdLineParameters.hpp>
#include <duke/engine/Player.hpp>

static const size_t startFrame = 90000;

using namespace duke;

const CmdLineParameters gDefault(0, nullptr);

Timeline getTimeline() {
	Track track;
	track.add(startFrame, Clip { 1 });
	track.add(startFrame + 1, Clip { 1 });
	track.add(startFrame + 2, Clip { 1 });
	track.add(startFrame + 3, Clip { 1 });
	return {track};
}

TEST(Player,time) {
	Player player(gDefault);
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
	Player player(gDefault);
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
	Player player(gDefault);
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
	Player player(gDefault);
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
	Player player(gDefault);
	Track track;
	track.add(0, Clip { 1 });
	track.add(1, Clip { 1 });
	track.add(2, Clip { 1 });
	track.add(3, Clip { 1 });
	const auto timeline = Timeline { track };
	const auto range = timeline.getRange();
	auto step = [&]() {player.offsetPlaybackTime(player.getFrameDuration());};
	EXPECT_EQ(range, Range (0,3));
	player.load(timeline, FrameDuration::PAL);
	EXPECT_EQ(Player::LOOP, player.getPlaybackMode());
	// moving one frame
	player.setPlaybackSpeed(-1);
	step(); // reverse should go last frame
	EXPECT_EQ(FrameIndex(3), player.getCurrentFrame());
	player.setPlaybackSpeed(1);
	step(); // forward should go first frame
	EXPECT_EQ(FrameIndex(0), player.getCurrentFrame());
	step();
	EXPECT_EQ(FrameIndex(1), player.getCurrentFrame());
	step();
	EXPECT_EQ(FrameIndex(2), player.getCurrentFrame());
	step();
	EXPECT_EQ(FrameIndex(3), player.getCurrentFrame());
	step();
	EXPECT_EQ(FrameIndex(0), player.getCurrentFrame());
}

TEST(Player,looping3) {
	Player player(gDefault);
	Track track;
	track.add(0, Clip { 2 });
	const auto timeline = Timeline { track };
	const auto range = timeline.getRange();
	EXPECT_EQ(range, Range (0,1));
	player.load(timeline, FrameDuration(1));
	EXPECT_EQ(Player::LOOP, player.getPlaybackMode());
	EXPECT_EQ(FrameIndex(0), player.getCurrentFrame());
	player.setPlaybackSpeed(1);
	player.offsetPlaybackTime(Time(1)); // 1s
	EXPECT_EQ(FrameIndex(1), player.getCurrentFrame().round());
	player.offsetPlaybackTime(Time(1, 3)); // 1.333s
	EXPECT_EQ(FrameIndex(1), player.getCurrentFrame().round());
	player.offsetPlaybackTime(Time(1, 3)); // 1.666s
	EXPECT_EQ(FrameIndex(1), player.getCurrentFrame().round());
	player.offsetPlaybackTime(Time(1, 3)); // 2s => looping
	EXPECT_EQ(FrameIndex(0), player.getCurrentFrame().round());
	// backward
	player.cue(0);
	player.setPlaybackSpeed(1);
	EXPECT_EQ(Time(0), player.getPlaybackTime());
	player.offsetPlaybackTime(Time(-1, 3)); // looping 1.666s
	EXPECT_EQ(Time(5,3), player.getPlaybackTime());
	EXPECT_EQ(FrameIndex(1), player.getCurrentFrame().round());
	player.offsetPlaybackTime(Time(-2, 3)); // 1s
	EXPECT_EQ(Time(1), player.getPlaybackTime());
	EXPECT_EQ(FrameIndex(1), player.getCurrentFrame().round());
	player.offsetPlaybackTime(Time(-1, 1000)); // 0.999s
	EXPECT_EQ(Time(999,1000), player.getPlaybackTime());
	EXPECT_EQ(FrameIndex(0), player.getCurrentFrame().round());
}

TEST(Player,forwardThenBackToStart) {
	Player player(gDefault);
	Track track;
	track.add(0, Clip { 200 });
	const auto timeline = Timeline { track };
	player.load(timeline, FrameDuration::PAL);
	player.setPlaybackSpeed(1);
	for (size_t i = 0; i < 4; ++i)
		player.offsetPlaybackTime(FrameDuration::PAL);
	EXPECT_EQ(FrameIndex(4), player.getCurrentFrame().round());
	player.setPlaybackSpeed(-1);
	for (size_t i = 0; i < 3; ++i)
		player.offsetPlaybackTime(FrameDuration::PAL);
	player.offsetPlaybackTime(FrameDuration::PAL);
	EXPECT_EQ(FrameIndex(0), player.getCurrentFrame().round());
}

TEST(Player,loopingWithHugeStep) {
	// if in looping mode with offset greater than timeline period, just doing nothing
	Player player(gDefault);
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
