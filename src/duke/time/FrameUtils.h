#ifndef FRAMEUTILS_H_
#define FRAMEUTILS_H_

#include <duke/Rational.h>

#include <chrono>

typedef rational<uint64_t> BaseRational;

typedef BaseRational Frame;

struct Time: BaseRational {
	Time(const BaseRational rational) :
			BaseRational(rational) {
	}
	Time(const uint64_t num, const uint64_t den) :
			BaseRational(num, den) {
	}
	Time(const std::chrono::microseconds value) :
			BaseRational(value.count(), std::micro::den) {
	}
	std::chrono::microseconds asMicroseconds() const {
		const uint64_t approx = uint64_t((double(numerator()) / denominator() * std::micro::den) + .5);
		return std::chrono::microseconds(approx);
	}
};

struct PlaybackPeriod: BaseRational {
	PlaybackPeriod(uint64_t num, uint64_t den) :
			BaseRational(num, den) {
	}
	const static PlaybackPeriod FILM;
	const static PlaybackPeriod PAL;
	const static PlaybackPeriod NTSC;
};

Time frameToTime(const uint32_t frame, const PlaybackPeriod &period);
Frame timeToFrame(Time time, const PlaybackPeriod &period);

#endif /* FRAMEUTILS_H_ */
