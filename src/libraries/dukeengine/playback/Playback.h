#ifndef PLAYBACK_H_
#define PLAYBACK_H_

#include <boost/chrono.hpp>

namespace playback {

typedef ::boost::chrono::high_resolution_clock::duration duration;
typedef ::boost::chrono::high_resolution_clock::time_point time_point;

extern ::boost::chrono::high_resolution_clock s_Clock;

// forward declaration
class PlaybackState;

/**
 * Returns the time for a frame in ns considering a given framerate
 */
boost::chrono::nanoseconds nsPerFrame(double frameRate);
boost::chrono::nanoseconds nsPerFrame(uint32_t numerator, uint32_t denominator);

/**
 * Represent a continuous state ( playback speed is constant, frame are adjacent from minFrame to maxFrame)
 * This class is not intended to be used directly, use PlaybackState instead.
 */
struct ContinuousPlaybackState {

    ContinuousPlaybackState(size_t newFrame, duration nsPerFrame, uint32_t minFrame, uint32_t maxFrame);

    time_point presentationTimeFor(uint32_t newFrame) const;

    bool adjustCurrentFrame(bool &frameMissed);

private:
    friend struct PlaybackState;

    bool frameOverrun() const;
    bool stepFrame();

    // those should be const but are not because of the operator=
    duration m_NsPerFrame;
    uint32_t m_EpochFrame;
    time_point m_EpochTime;
    uint32_t m_MinFrame;
    uint32_t m_MaxFrame;
    // mutable field
    uint32_t m_Frame;
};

/**
 * Represents the playback state, it can loop, cue and play
 */
struct PlaybackState {
    PlaybackState();
    PlaybackState(duration nsPerFrame, uint32_t minFrame, uint32_t maxFrame, bool loop);

    inline void waitForPresentation() const {
        while (!shouldPresent())
            ;
    }

    inline bool shouldPresent() const {
        return true;
//                return isPlaying() ? m_State.frameOverrun() : true;
    }

    inline uint32_t frame() const {
        return m_State.m_Frame;
    }

    inline time_point epochTime() const {
        return m_State.m_EpochTime;
    }

    inline uint32_t epochFrame() const {
        return m_State.m_EpochFrame;
    }

    inline duration playlistTime(uint32_t newFrame) const {
        return m_NsPerFrame * newFrame;
    }

    duration playlistTime() const {
        return playlistTime(m_State.m_EpochFrame) + (s_Clock.now() - epochTime());
    }

    bool adjustCurrentFrame();

    inline void stop() {
        cue(frame());
    }

    inline void cue(uint32_t frame) {
        play(frame, 0);
    }

    void play(uint32_t frame, int32_t speed = 1);

    inline bool isPlaying() const {
        return m_Speed != 0;
    }

    inline int32_t getSpeed() const {
        return m_Speed;
    }

private:

    inline duration nsPerFrame() const {
        return m_Speed == 0 ? duration::zero() : m_NsPerFrame / m_Speed;
    }

    // those should be const but are not because of the operator=
    bool m_Loop;
    uint32_t m_MinFrame;
    uint32_t m_MaxFrame;
    duration m_NsPerFrame;
    // mutable field
    int32_t m_Speed;
    ContinuousPlaybackState m_State;
};

}

#endif /* PLAYBACK_H_ */
