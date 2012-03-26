#ifndef PLAYBACK_H_
#define PLAYBACK_H_

#include <sequence/Range.h>

#include <boost/chrono.hpp>

namespace playback {

extern ::boost::chrono::high_resolution_clock s_Clock;

/**
 * Returns the time for a frame in ns considering a given framerate
 */
boost::chrono::nanoseconds nsPerFrame(double frameRate);
boost::chrono::nanoseconds nsPerFrame(unsigned int numerator, unsigned int denominator);

enum PlaybackType {
    RENDER, REALTIME_NO_SKIP, REALTIME
};

struct Playback {
    Playback();
    bool shouldPresent();
    bool adjustCurrentFrame();
    void setType(PlaybackType _type);
    void init(const sequence::Range &range, bool loop,boost::chrono::high_resolution_clock::duration nsPerFrame );
    void play(unsigned int frame, int speed = 1);
    boost::chrono::high_resolution_clock::duration playlistTime() const;
    unsigned int frame() const;
    int getSpeed() const;
    void stop() { play(frame(),0); }
    void cue(unsigned int frame) { play(frame,getSpeed()); }
    bool playing() const { return getSpeed()!=0; }
private:
    struct PlaybackImpl;
    std::auto_ptr<PlaybackImpl> m_Impl;
};

}  // namespace playback

#endif /* PLAYBACK_H_ */
