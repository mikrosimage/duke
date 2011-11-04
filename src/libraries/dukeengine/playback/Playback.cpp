#include "Playback.h"

using namespace std;
using namespace boost::chrono;

namespace playback {

// globals
high_resolution_clock s_Clock;
const duration zero(high_resolution_clock::duration::zero());

/**
 * Returns the time for a frame in ns considering a given framerate
 */
nanoseconds nsPerFrame(uint32_t numerator, uint32_t denominator) {
    assert(denominator!=0);
    if (numerator == 0)
        return zero;
    return nanoseconds(seconds(denominator)) / numerator;
}

nanoseconds nsPerFrame(double frameRate) {
    if (frameRate == 0)
        return zero;
    const int64_t nsPerSecond = nanoseconds(seconds(1)).count();
    const int64_t nsPerFrame = nsPerSecond / frameRate;
    return nanoseconds(nsPerFrame);
}

ContinuousPlaybackState::ContinuousPlaybackState(size_t newFrame, high_resolution_clock::duration nsPerFrame, uint32_t minFrame, uint32_t maxFrame) :
    m_NsPerFrame(nsPerFrame), m_EpochFrame(newFrame), m_EpochTime(s_Clock.now()), m_MinFrame(std::min(minFrame, maxFrame)), m_MaxFrame(std::max(minFrame, maxFrame)),
            m_Frame(newFrame) {
}

bool ContinuousPlaybackState::frameOverrun() const {
    return false;
//    return presentationTimeFor(m_Frame) < s_Clock.now();
}

high_resolution_clock::time_point ContinuousPlaybackState::presentationTimeFor(uint32_t newFrame) const {
    const int64_t frameDifferences = int64_t(newFrame) - int64_t(m_EpochFrame);
    return m_EpochTime + frameDifferences * m_NsPerFrame;
}

bool ContinuousPlaybackState::adjustCurrentFrame(bool &frameMissed) {
    frameMissed = false;
    if (m_NsPerFrame == zero)
        return false;

    bool secondStep = false;
    do {
        if (stepFrame())
            return true;
        frameMissed |= secondStep; // if more than one step we missed a frame
        secondStep = true;
    } while (frameOverrun());
    return false;
}

/**
 * return true if a bound is reached
 */
inline bool ContinuousPlaybackState::stepFrame() {
    assert(m_NsPerFrame!=zero); // stop state is not allowed, must be either forward or backward here
    if (m_NsPerFrame > zero) { // forward
        if (m_Frame >= m_MaxFrame)
            return true;
        else
            ++m_Frame;
    } else { // backward
        if (m_Frame <= m_MinFrame)
            return true;
        else
            --m_Frame;
    }
    return false;
}

PlaybackState::PlaybackState() :
    m_Loop(true), m_MinFrame(0), m_MaxFrame(0), m_NsPerFrame(zero), m_Speed(0), m_State(m_MinFrame, zero, m_MinFrame, m_MaxFrame) {
}

PlaybackState::PlaybackState(high_resolution_clock::duration nsPerFrame, uint32_t minFrame, uint32_t maxFrame, bool loop) :
    m_Loop(loop), m_MinFrame(minFrame), m_MaxFrame(maxFrame), m_NsPerFrame(nsPerFrame), m_Speed(0), m_State(m_MinFrame, zero, m_MinFrame, m_MaxFrame) {
}

bool PlaybackState::adjustCurrentFrame() {
    bool frameMissed = false;
    if (isPlaying() && m_State.adjustCurrentFrame(frameMissed)) { // bound reached
        if (m_Loop) {
            const uint32_t otherBound = frame() == m_MaxFrame ? m_MinFrame : m_MaxFrame;
            m_State = ContinuousPlaybackState(otherBound, nsPerFrame(), m_MinFrame, m_MaxFrame);
        } else {
            stop();
        }
    }
    return frameMissed;
}

void PlaybackState::play(uint32_t frame, int32_t speed) {
    m_Speed = speed;
    m_State = ContinuousPlaybackState(frame, nsPerFrame(), m_MinFrame, m_MaxFrame);
}

} // namespace playback
