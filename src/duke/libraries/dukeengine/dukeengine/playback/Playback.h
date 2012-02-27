#ifndef PLAYBACK_H_
#define PLAYBACK_H_

#include <boost/chrono.hpp>

namespace playback {

typedef ::boost::chrono::high_resolution_clock::duration duration;
typedef ::boost::chrono::high_resolution_clock::time_point time_point;

extern ::boost::chrono::high_resolution_clock s_Clock;

// forward declaration
struct PlaybackState;

/**
 * Returns the time for a frame in ns considering a given framerate
 */
boost::chrono::nanoseconds nsPerFrame(double frameRate);
boost::chrono::nanoseconds nsPerFrame(unsigned int numerator, unsigned int denominator);

/**
 * Represent a continuous state ( playback speed is constant, frame are adjacent from minFrame to maxFrame)
 * This class is not intended to be used directly, use PlaybackState instead.
 */
struct ContinuousPlaybackState {

    ContinuousPlaybackState(size_t newFrame, duration nsPerFrame, unsigned int minFrame, unsigned int maxFrame);

    time_point presentationTimeFor(unsigned int newFrame) const;

    bool adjustCurrentFrame(bool &frameMissed);

private:
    friend struct PlaybackState;

    bool frameOverrun() const;
    bool stepFrame();

    // those should be const but are not because of the operator=
    duration m_NsPerFrame;
    unsigned int m_EpochFrame;
    time_point m_EpochTime;
    unsigned int m_MinFrame;
    unsigned int m_MaxFrame;
    // mutable field
    unsigned int m_Frame;
};

/**
 * Represents the playback state, it can loop, cue and play
 */
struct PlaybackState {
    PlaybackState();
    PlaybackState(duration nsPerFrame, unsigned int minFrame, unsigned int maxFrame, bool loop);

    inline bool shouldPresent() const {
//        return true;
        return isPlaying() ? m_State.frameOverrun() : true;
    }

    inline unsigned int frame() const { return m_State.m_Frame; }
    duration playlistTime() const { return playlistTime(m_State.m_EpochFrame) + (s_Clock.now() - epochTime()); }
    bool adjustCurrentFrame();

    inline void cue(unsigned int frame) { play(frame, 0); }
    void play(unsigned int frame, int speed = 1);

    inline bool isPlaying() const { return m_Speed != 0; }
    inline int32_t getSpeed() const { return m_Speed; }


private:
    inline void stop() { cue(frame()); }
    inline duration nsPerFrame() const { return m_Speed == 0 ? duration::zero() : m_NsPerFrame / m_Speed; }
    inline time_point epochTime() const { return m_State.m_EpochTime; }
    inline duration playlistTime(unsigned int newFrame) const { return m_NsPerFrame * newFrame; }

//    inline void waitForPresentation() const {
//        while (!shouldPresent())
//            ;
//    }
//    inline unsigned int epochFrame() const {
//        return m_State.m_EpochFrame;
//    }

    // those should be const but are not because of the operator=
    bool m_Loop;
    unsigned int m_MinFrame;
    unsigned int m_MaxFrame;
    duration m_NsPerFrame;
    // mutable field
    int m_Speed;
    ContinuousPlaybackState m_State;
};

}

#endif /* PLAYBACK_H_ */
