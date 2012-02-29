/*
 * Playback.cpp
 *
 *  Created on: 29 févr. 2012
 *      Author: Guillaume Chatelet
 */

#include "Playback.h"

using namespace std;

using namespace boost::chrono;

high_resolution_clock s_Clock;

namespace playback {

/**
 * Returns the time for a frame in ns considering a given framerate
 */
nanoseconds nsPerFrame(unsigned int numerator, unsigned int denominator) {
    assert(denominator!=0);
    if (numerator == 0)
        return high_resolution_clock::duration::zero();
    return nanoseconds(seconds(denominator)) / numerator;
}

nanoseconds nsPerFrame(double frameRate) {
    if (frameRate == 0)
        return high_resolution_clock::duration::zero();
    const int64_t nsPerSecond = nanoseconds(seconds(1)).count();
    const int64_t nsPerFrame = nsPerSecond / frameRate;
    return nanoseconds(nsPerFrame);
}

struct State {
    int speed;
    unsigned int currentFrame;
    bool isLooping;
    sequence::Range range;
    high_resolution_clock::duration nsPerFrame;
    high_resolution_clock::time_point epoch;
    unsigned int epochFrame;

    State() : speed(0), currentFrame(0), isLooping(false), epochFrame(0) {
    }

    void adjustCurrentFrame() {
        if (speed == 0)
            return;
        const sequence::Range::MoveResult newFrame = isLooping ? //
                        range.offsetLoopFrame(currentFrame, speed) : //
                        range.offsetClampFrame(currentFrame, speed);
        if (newFrame.second) {
            epoch = s_Clock.now() + nsPerDisplay();
            if (!isLooping)
                speed = 0;
        }
        currentFrame = newFrame.first;
    }

    void setNewContinuum(unsigned _frame, int _speed) {
        epoch = s_Clock.now();
        epochFrame = currentFrame = _frame;
        speed = _speed;
    }

    void init(const sequence::Range &_range, bool loop, high_resolution_clock::duration _nsPerFrame) {
        range = _range;
        isLooping = loop;
        nsPerFrame = _nsPerFrame;
        setNewContinuum(range.first, 0);
    }

    high_resolution_clock::duration playlistTime() const {
        return playlistTime(epochFrame) + (s_Clock.now() - epoch);
    }

    high_resolution_clock::time_point presentationTime() const {
        return presentationTime(currentFrame);
    }

private:
    high_resolution_clock::duration duration(unsigned frames) const {
        return frames * nsPerFrame;
    }

    high_resolution_clock::duration playlistTime(unsigned atFrame) const {
        return duration(atFrame - range.first);
    }

    high_resolution_clock::duration nsPerDisplay() const {
        return speed == 0 ? high_resolution_clock::duration::zero() : nsPerFrame / speed;
    }

    high_resolution_clock::time_point presentationTime(unsigned frame) const {
        const int64_t offsetFromEpochToFrame = int64_t(frame) - int64_t(epochFrame);
        if (speed == 0)
            return s_Clock.now();
        const high_resolution_clock::duration durationFromEpochToFrame = offsetFromEpochToFrame * (nsPerFrame / speed);
        return epoch + durationFromEpochToFrame;
    }
};

struct IPlayback {
    virtual ~IPlayback() {}
    virtual bool shouldPresent() const = 0;
    virtual bool adjustCurrentFrame() = 0;
};

struct RenderPlayback : public IPlayback {
    RenderPlayback(State &state) : state(state) { }
    virtual bool shouldPresent() const {
        return true;
    }
    virtual bool adjustCurrentFrame() {
        state.adjustCurrentFrame();
        return false;
    }
protected:
    State &state;
};

struct RealtimeNoSkipPlayback : public RenderPlayback {
    RealtimeNoSkipPlayback(State &state) : RenderPlayback(state) { }
    bool frameOverrun() const {
        return state.presentationTime() < s_Clock.now();
    }
    virtual bool shouldPresent() const {
        return state.speed != 0 ? frameOverrun() : true;
    }
    virtual bool adjustCurrentFrame() {
        state.adjustCurrentFrame();
        return frameOverrun();
    }
};

struct RealtimeSkipPlayback : public RealtimeNoSkipPlayback {
    RealtimeSkipPlayback(State &state) : RealtimeNoSkipPlayback(state) { }
    virtual bool adjustCurrentFrame() {
        unsigned count = 0;
        for (; frameOverrun(); ++count)
            state.adjustCurrentFrame();
        return count >= 2;
    }
};

struct PlaybackImpl {
    PlaybackImpl() : type(REALTIME), pb_render(state), pb_realtimeNoSkip(state), pb_realtime(state) {
    }
    inline IPlayback& select() {
        switch (type) {
            case RENDER:
                return pb_render;
            case REALTIME_NO_SKIP:
                return pb_realtimeNoSkip;
            case REALTIME:
                return pb_realtime;
        }
        throw runtime_error("unknown case");
    }
    State state;
    PlaybackType type;
    RenderPlayback pb_render;
    RealtimeNoSkipPlayback pb_realtimeNoSkip;
    RealtimeSkipPlayback pb_realtime;
};

Playback::Playback() : m_Impl(new PlaybackImpl) { }

bool Playback::shouldPresent() {
    return m_Impl->select().shouldPresent();
}

bool Playback::adjustCurrentFrame() {
    return m_Impl->select().adjustCurrentFrame();
}

void Playback::setType(PlaybackType _type) {
    m_Impl->type = _type;
}

void Playback::init(const sequence::Range &range, bool loop, boost::chrono::high_resolution_clock::duration nsPerFrame) {
    m_Impl->state.init(range, loop, nsPerFrame);
}

void Playback::play(unsigned int frame, int speed) {
    m_Impl->state.setNewContinuum(frame, speed);
}

boost::chrono::high_resolution_clock::duration Playback::playlistTime() const {
    return m_Impl->state.playlistTime();
}

unsigned int Playback::frame() const {
    return m_Impl->state.currentFrame;
}

int Playback::getSpeed() const {
    return m_Impl->state.speed;
}

} // namespace playback
