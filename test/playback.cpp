#include <gtest/gtest.h>

#include <duke/engine/Player.h>

static const size_t startFrame = 90000;

Timeline getTimeline() {
	Timeline timeline;
	timeline.emplace_back();
	Track &track = timeline.back();
	track.add(startFrame, Clip { 101 });
	return timeline;
}

TEST(Player,time) {
	Player player;
	EXPECT_EQ(Frame(), player.getCurrentFrame());
	player.setPlaybackTime(10); // go to ten seconds
	EXPECT_EQ(Frame(25*10), player.getCurrentFrame());
	player.load(getTimeline(), FrameDuration::PAL);
	// should be at the beginning of the timeline
	EXPECT_EQ(Time(3600), player.getPlaybackTime());
	EXPECT_EQ(Frame(startFrame ), player.getCurrentFrame());
	player.setPlaybackTime(3610); // go to ten seconds
	EXPECT_EQ(Frame(startFrame + 10*25), player.getCurrentFrame());
}

TEST(Player,playback) {
	Player player;
	player.load(getTimeline(), FrameDuration::PAL);
	player.setPlaybackMode(Player::CONTINUE);
	// playing
	player.setPlaybackSpeed(1);
	player.offsetPlaybackTime(player.getFrameDuration());
	const auto currentFrame = player.getCurrentFrame();
	EXPECT_EQ(Frame(startFrame + 1), currentFrame);

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
