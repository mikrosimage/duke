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

struct TimeCycleEvaluator {
	TimeCycleEvaluator(int64_t duration = 300);
	double getInterpolatedTime(int64_t time) const;

	int64_t duration;
	int64_t repeatCount = 0; // RepeatCount::INFINITE;
	RepeatMode repeatMode = RepeatMode::RESTART;
	int64_t startTime = 0;
private:
	double getFractionWithMode(int64_t elapsed, int64_t count) const;
	double getReverseFraction(int64_t elapsed) const;
	double getFraction(int64_t elapsed) const;
	bool isInfinite() const;
};

class TimeInterpolator {
public:
	virtual ~TimeInterpolator() = 0;
	virtual double getInterpolation(double input) const = 0;
};

template<typename T>
struct Animation: TimeCycleEvaluator {
	Animation() :
			TimeCycleEvaluator(), startValue(), endValue() {
	}
	Animation(int32_t duration, T start, T end) :
			TimeCycleEvaluator(duration), startValue(start), endValue(end) {
	}

	T getAnimatedValue(int64_t time, const TimeInterpolator &interpolator) const {
		const double normalizedTime = getInterpolatedTime(time);
		const double interpolatedTime = interpolator.getInterpolation(normalizedTime);
		return startValue * (1 - interpolatedTime) + endValue * interpolatedTime;
	}

	T startValue;
	T endValue;
};

enum class EasingCurve {
	Linear, //
	InQuad, OutQuad, InOutQuad, OutInQuad, //
	InCubic, OutCubic, InOutCubic, OutInCubic, //
	InQuart, OutQuart, InOutQuart, OutInQuart, //
	InQuint, OutQuint, InOutQuint, OutInQuint, //
	InSine, OutSine, InOutSine, OutInSine, //
	InExpo, OutExpo, InOutExpo, OutInExpo, //
	InCirc, OutCirc, InOutCirc, OutInCirc, //
	InElastic, OutElastic, InOutElastic, OutInElastic, //
	InBack, OutBack, InOutBack, OutInBack, //
	InBounce, OutBounce, InOutBounce, OutInBounce
};

class EasingCurveTimeInterpolator: public TimeInterpolator {
public:
	EasingCurveTimeInterpolator(EasingCurve type, double period = .3, double amplitude = 1, double overshoot = 1.70158);
	virtual double getInterpolation(double input) const;
	EasingCurve type;
	double period;
	double amplitude;
	double overshoot;
};

} /* namespace duke */
#endif /* ANIMATION_H_ */
