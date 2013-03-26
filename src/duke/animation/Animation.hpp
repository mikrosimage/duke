#pragma once

#include <cstdint>

namespace duke {

enum class RepeatMode
	: unsigned char {RESTART, REVERSE
};

enum RepeatCount
	: int64_t {
		INFINITE = -1
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

struct AnimationData {
	int64_t duration = 300;
	int64_t repeatCount = 0; // RepeatCount::INFINITE;
	RepeatMode repeatMode = RepeatMode::RESTART;
	int64_t startTime = 0;
	EasingCurve type = EasingCurve::Linear;
	double period = .3;
	double amplitude = 1;
	double overshoot = 1.70158;
};

double getCycleValue(const AnimationData&, int64_t time);
double interpolateCycleValue(const AnimationData&, double input);

template<typename T>
T interpolateValue(const AnimationData& data, T startValue, T endValue, int64_t time) {
	const double normalizedTime = getCycleValue(data, time);
	const double interpolatedTime = interpolateCycleValue(data, normalizedTime);
	return startValue * (1 - interpolatedTime) + endValue * interpolatedTime;
}

} /* namespace duke */
