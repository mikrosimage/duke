#ifndef PLAYER_H_
#define PLAYER_H_

#include <duke/time/FrameUtils.h>
#include <duke/time/Timeline.h>
#include <duke/NonCopyable.h>

struct Player: public NonCopyable {
	enum Mode {
		CONTINUE, LOOP, STOP
	};

	Player();

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

private:
	Timeline m_Timeline;
	Range m_TimelineRange;
	Time m_FirstFrameTime, m_LastFrameTime;
	Time m_PlaybackTime;
	FrameDuration m_FrameDuration;
	int m_PlaybackSpeed;
	Mode m_PlaybackMode;
};

#endif /* PLAYER_H_ */
