/*
 * Animation.h
 *
 *  Created on: Jan 25, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <cstdint>

namespace duke {

enum class RepeatMode
    : unsigned char {RESTART, REVERSE
};

enum RepeatCount
    : int64_t {
        INFINITE = -1
};

struct TimeInterpolator {

    TimeInterpolator(int64_t duration) :
                    duration(duration) {
    }

    double getInterpolatedTime(int64_t time) const {
        const int64_t elapsed = time - startTime;
        const int64_t forwardElapsed = elapsed < 0 ? 0 : elapsed;
        const int64_t repeatedCount = forwardElapsed / duration;
        const int64_t elapsedFraction = forwardElapsed - (repeatedCount * duration);
        if (!isInfinite() && repeatedCount > repeatCount)
            return 1;
        return getFractionWithMode(elapsedFraction, repeatedCount);
    }

    bool isOdd(int64_t count) const {
        return (count & 0x01) > 0;
    }

    double getFractionWithMode(int64_t elapsed, int64_t count) const {
        return repeatMode == RepeatMode::REVERSE && isOdd(count) ? getReverseFraction(elapsed) : getFraction(elapsed);
    }

    double getReverseFraction(int64_t elapsed) const {
        return 1 - getFraction(elapsed);
    }

    double getFraction(int64_t elapsed) const {
        return double(elapsed) / duration;
    }

    bool isInfinite() const {
        return repeatCount == RepeatCount::INFINITE;
    }

    int64_t duration;
    int64_t repeatCount = 0; // RepeatCount::INFINITE;
    RepeatMode repeatMode = RepeatMode::RESTART;
    int64_t startTime = 0;
};

template<typename T>
struct Animation : TimeInterpolator {

    Animation(int32_t duration, T start, T end) :
                    TimeInterpolator(duration), startValue(start), endValue(end) {
    }

    T startValue;
    T endValue;
};

}  // namespace duke

#endif /* ANIMATION_H_ */
