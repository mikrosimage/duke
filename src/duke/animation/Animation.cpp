/*
 * Animation.cpp
 *
 *  Created on: Jan 25, 2013
 *      Author: Guillaume Chatelet
 */

#include "Animation.h"

#include <cmath>
#include <stdexcept>

namespace duke {

static bool isOdd(int64_t count) {
	return (count & 0x01) > 0;
}

TimeCycleEvaluator::TimeCycleEvaluator(int64_t duration) :
		duration(duration) {
}

double TimeCycleEvaluator::getInterpolatedTime(int64_t time) const {
	const int64_t elapsed = time - startTime;
	const int64_t forwardElapsed = elapsed < 0 ? 0 : elapsed;
	const int64_t repeatedCount = forwardElapsed / duration;
	const int64_t elapsedFraction = forwardElapsed - (repeatedCount * duration);
	if (!isInfinite() && repeatedCount > repeatCount)
		return 1;
	return getFractionWithMode(elapsedFraction, repeatedCount);
}

double TimeCycleEvaluator::getFractionWithMode(int64_t elapsed, int64_t count) const {
	return repeatMode == RepeatMode::REVERSE && isOdd(count) ? getReverseFraction(elapsed) : getFraction(elapsed);
}

double TimeCycleEvaluator::getReverseFraction(int64_t elapsed) const {
	return 1 - getFraction(elapsed);
}

double TimeCycleEvaluator::getFraction(int64_t elapsed) const {
	return double(elapsed) / duration;
}

bool TimeCycleEvaluator::isInfinite() const {
	return repeatCount == RepeatCount::INFINITE;
}

TimeInterpolator::~TimeInterpolator() {
}

EasingCurveTimeInterpolator::EasingCurveTimeInterpolator(EasingCurve type, double period, double amplitude, double overshoot) :
		type(type), period(period), amplitude(amplitude), overshoot(overshoot) {
}
struct quadratic {
	static inline double apply(double t) {
		return t * t;
	}
};
struct cubic {
	static inline double apply(double t) {
		return quadratic::apply(t) * t;
	}
};
struct quartic {
	static inline double apply(double t) {
		const double quad = quadratic::apply(t);
		return quad * quad;
	}
};
struct quintic {
	static inline double apply(double t) {
		return quartic::apply(t) * t;
	}
};
struct sine {
	static inline double apply(double t) {
		return -::cos(t * M_PI_2) + 1;
	}
};
struct expo {
	static inline double apply(double t) {
		return (t == 0 || t == 1.0) ? t : ::pow(2.0, 10 * (t - 1)) - 0.001;
	}
};
struct circ {
	static inline double apply(double t) {
		return -(::sqrt(1 - t * t) - 1);
	}
};

struct elastic_reversed {
	static inline double apply(double t, double amplitude, double period) {
		if (t == 0)
			return 0;
		if (t == 1)
			return 1;

		double s;
		if (amplitude < 1) {
			amplitude = 1;
			s = period / 4.0f;
		} else {
			s = period / (2 * M_PI) * ::asin(1 / amplitude);
		}
		return (amplitude * ::pow(2.0f, -10 * t) * ::sin((t - s) * (2 * M_PI) / period) + 1);
	}
};

struct bounce_reverse {
	static inline double apply(double t, double amplitude) {
		if (t == 1.0)
			return 1;
		if (t < (4 / 11.0)) {
			return 7.5625 * t * t;
		} else if (t < (8 / 11.0)) {
			t -= (6 / 11.0);
			return -amplitude * (1. - (7.5625 * t * t + .75)) + 1;
		} else if (t < (10 / 11.0)) {
			t -= (9 / 11.0);
			return -amplitude * (1. - (7.5625 * t * t + .9375)) + 1;
		} else {
			t -= (21 / 22.0);
			return -amplitude * (1. - (7.5625 * t * t + .984375)) + 1;
		}
	}
};

struct back {
	static inline double apply(double t, double overshoot) {
		return t * t * ((overshoot + 1) * t - overshoot);
	}
};

template<typename T, typename ... Params>
struct in {
	static inline double apply(double t, Params ... args) {
		return T::apply(t, args...);
	}
};
template<typename T, typename ... Params>
struct out {
	static inline double apply(double t, Params ... args) {
		return 1 - T::apply(1 - t, args...);
	}
};
template<typename T, typename ... Params>
struct inout {
	static inline double apply(double t, Params ... args) {
		if (t <= .5)
			return in<T, Params...>::apply(t * 2, args...) / 2;
		return out<T, Params...>::apply(2 * t - 1, args...) / 2 + .5;
	}
};
template<typename T, typename ... Params>
struct outin {
	static inline double apply(double t, Params ... args) {
		if (t <= .5)
			return out<T, Params...>::apply(t * 2, args...) / 2;
		return in<T, Params...>::apply(2 * t - 1, args...) / 2 + .5;
	}
};

double EasingCurveTimeInterpolator::getInterpolation(double t) const {
	switch (type) {
	case EasingCurve::Linear:
		return t;
	case EasingCurve::InQuad:
		return in<quadratic>::apply(t);
	case EasingCurve::OutQuad:
		return out<quadratic>::apply(t);
	case EasingCurve::InOutQuad:
		return inout<quadratic>::apply(t);
	case EasingCurve::OutInQuad:
		return outin<quadratic>::apply(t);
	case EasingCurve::InCubic:
		return in<cubic>::apply(t);
	case EasingCurve::OutCubic:
		return out<cubic>::apply(t);
	case EasingCurve::InOutCubic:
		return inout<cubic>::apply(t);
	case EasingCurve::OutInCubic:
		return outin<cubic>::apply(t);
	case EasingCurve::InQuart:
		return in<quartic>::apply(t);
	case EasingCurve::OutQuart:
		return out<quartic>::apply(t);
	case EasingCurve::InOutQuart:
		return inout<quartic>::apply(t);
	case EasingCurve::OutInQuart:
		return outin<quartic>::apply(t);
	case EasingCurve::InQuint:
		return in<quintic>::apply(t);
	case EasingCurve::OutQuint:
		return out<quintic>::apply(t);
	case EasingCurve::InOutQuint:
		return inout<quintic>::apply(t);
	case EasingCurve::OutInQuint:
		return outin<quintic>::apply(t);
	case EasingCurve::InSine:
		return in<sine>::apply(t);
	case EasingCurve::OutSine:
		return out<sine>::apply(t);
	case EasingCurve::InOutSine:
		return inout<sine>::apply(t);
	case EasingCurve::OutInSine:
		return outin<sine>::apply(t);
	case EasingCurve::InExpo:
		return in<expo>::apply(t);
	case EasingCurve::OutExpo:
		return out<expo>::apply(t);
	case EasingCurve::InOutExpo:
		return inout<expo>::apply(t);
	case EasingCurve::OutInExpo:
		return outin<expo>::apply(t);
	case EasingCurve::InCirc:
		return in<circ>::apply(t);
	case EasingCurve::OutCirc:
		return out<circ>::apply(t);
	case EasingCurve::InOutCirc:
		return inout<circ>::apply(t);
	case EasingCurve::OutInCirc:
		return outin<circ>::apply(t);
	case EasingCurve::InElastic:
		return out<elastic_reversed, double, double>::apply(t, amplitude, period);
	case EasingCurve::OutElastic:
		return in<elastic_reversed, double, double>::apply(t, amplitude, period);
	case EasingCurve::InOutElastic:
		return outin<elastic_reversed, double, double>::apply(t, amplitude, period);
	case EasingCurve::OutInElastic:
		return inout<elastic_reversed, double, double>::apply(t, amplitude, period);
	case EasingCurve::InBack:
		return in<back, double>::apply(t, overshoot);
	case EasingCurve::OutBack:
		return out<back, double>::apply(t, overshoot);
	case EasingCurve::InOutBack:
		return inout<back, double>::apply(t, overshoot);
	case EasingCurve::OutInBack:
		return outin<back, double>::apply(t, overshoot);
	case EasingCurve::InBounce:
		return out<bounce_reverse, double>::apply(t, amplitude);
	case EasingCurve::OutBounce:
		return in<bounce_reverse, double>::apply(t, amplitude);
	case EasingCurve::InOutBounce:
		return outin<bounce_reverse, double>::apply(t, amplitude);
	case EasingCurve::OutInBounce:
		return inout<bounce_reverse, double>::apply(t, amplitude);
	}
	throw std::runtime_error("Easing curve not yet implemented");
}

}
/* namespace duke */
