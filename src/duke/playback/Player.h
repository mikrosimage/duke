#ifndef PLAYER_H_
#define PLAYER_H_

#include <duke/timeline/Timeline.h>
#include <duke/NonCopyable.h>

struct Player: public NonCopyable {
	Player();
	~Player();

	void load(const Timeline& timeline) {
		m_Timeline = timeline;
		cue(m_Timeline.empty() ? 0 : m_Timeline.getRange().first);
	}
	void play(size_t frame, int rate = 1);
	void stop();
	inline void cue(size_t frame) {
		play(frame, 0);
	}
	inline size_t getCurrentFrame() const {
		return m_CurrentFrame;
	}
private:
	Timeline m_Timeline;
	size_t m_CurrentFrame;
	uint64_t m_ElapsedMicroseconds;
};

#endif /* PLAYER_H_ */
