#include "Player.hpp"
#include <duke/cmdline/CmdLineParameters.hpp>

namespace duke {

Player::Player(const CmdLineParameters &parameters) :
		m_TextureCache(parameters) {
}

void Player::load(const Timeline& timeline, const FrameDuration &duration) {
	m_TextureCache.load(timeline);
	m_TimelineRange = timeline.getRange();
	setFrameDuration(duration);
	cue(m_TimelineRange == Range::EMPTY ? 0 : m_TimelineRange.first);
}

void Player::setFrameDuration(const FrameDuration &duration) {
	m_FrameDuration = duration;
	if (m_TimelineRange == Range::EMPTY) {
		m_FirstFrameTime = m_LastFrameTime = 0;
	} else {
		m_FirstFrameTime = frameToTime(m_TimelineRange.first, getFrameDuration());
		m_LastFrameTime = frameToTime(m_TimelineRange.last, getFrameDuration());
		m_EndFrameTime = frameToTime(m_TimelineRange.last + 1, getFrameDuration());
	}
}

void Player::setPlaybackTime(const Time time) {
	m_PlaybackTime = time;
}

void Player::offsetPlaybackTime(const Time time) {
	const auto offset = time * m_PlaybackSpeed;
	if (m_PlaybackMode == CONTINUE) {
		m_PlaybackTime += offset;
		return;
	}
	if (m_TimelineRange == Range::EMPTY || offset == 0)
		return;
	const bool forward = offset > 0;
	const auto newTime = m_PlaybackTime + offset;
	const auto overshoot = forward ? newTime - m_EndFrameTime : m_FirstFrameTime - newTime;
	const bool overshooting = forward ? overshoot < 0 : overshoot <= 0;
	if (overshooting) {
		m_PlaybackTime += offset;
		return;
	}
	switch (m_PlaybackMode) {
	case STOP:
		m_PlaybackTime = forward ? m_LastFrameTime : m_FirstFrameTime;
		m_PlaybackSpeed = 0;
		break;
	case LOOP:
		if (overshoot > (m_EndFrameTime - m_FirstFrameTime))
			return;
		m_PlaybackTime = forward ? m_FirstFrameTime + overshoot : m_EndFrameTime - overshoot;
		break;
	default:
		throw std::logic_error("Not yet implemented");
	}
}

void Player::setPlaybackSpeed(int speed) {
	m_PlaybackSpeed = speed;
}

void Player::setPlaybackMode(const Mode mode) {
	m_PlaybackMode = mode;
}

void Player::stop() {
	setPlaybackSpeed(0);
}

void Player::cue(uint32_t frame) {
	m_PlaybackTime = m_FrameDuration * frame;
	stop();
}

void Player::cueRelative(int32_t frame) {
	setPlaybackSpeed(1);
	offsetPlaybackTime(m_FrameDuration * frame);
	stop();
}

FrameIndex Player::getCurrentFrame() const {
	return m_PlaybackTime / m_FrameDuration;
}

Time Player::getPlaybackTime() const {
	return m_PlaybackTime;
}

FrameDuration Player::getFrameDuration() const {
	return m_FrameDuration;
}

int Player::getPlaybackSpeed() const {
	return m_PlaybackSpeed;
}

Player::Mode Player::getPlaybackMode() const {
	return m_PlaybackMode;
}

const Timeline& Player::getTimeline() const {
	return m_TextureCache.getTimeline();
}

LoadedTextureCache& Player::getTextureCache() {
	return m_TextureCache;
}

}  // namespace duke
