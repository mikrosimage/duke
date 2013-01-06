#ifndef PLAYER_H_
#define PLAYER_H_

#include <duke/engine/Timeline.h>
#include <duke/time/FrameUtils.h>
#include <duke/NonCopyable.h>

struct Player: public NonCopyable {
	enum Mode {
		CONTINUE, LOOP, STOP
	};

	void load(const Timeline& timeline, const FrameDuration &duration);

	// frame based
	void stop();
	void cue(uint32_t frame);

	// time based
	void setPlaybackSpeed(int speed);
	void setPlaybackTime(const Time time);
	void offsetPlaybackTime(const Time time);
	void setPlaybackMode(const Mode mode);

	// accessors
	Frame getCurrentFrame() const;
	Time getPlaybackTime() const;
	FrameDuration getFrameDuration() const;
	int getPlaybackSpeed() const;
	Mode getPlaybackMode() const;
	const Timeline& getTimeline() const;

private:
	Timeline m_Timeline;
	Range m_TimelineRange = Range::EMPTY;
	Time m_FirstFrameTime;
	Time m_LastFrameTime;
	Time m_PlaybackTime;
	FrameDuration m_FrameDuration = FrameDuration::PAL;
	int m_PlaybackSpeed = 0;
	Mode m_PlaybackMode = LOOP;
};
#endif /* PLAYER_H_ */
