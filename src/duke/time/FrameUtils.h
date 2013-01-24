#ifndef FRAMEUTILS_H_
#define FRAMEUTILS_H_

#include <duke/Rational.h>

#include <chrono>

typedef rational<int64_t> BaseRational;

struct FrameIndex: public BaseRational {
	FrameIndex() :
			BaseRational() {
	}
	FrameIndex(const BaseRational rational) :
			BaseRational(rational) {
	}
	value_type round() const {
		return value_type((double(numerator()) / denominator()) + .5);
	}
	friend std::ostream& operator<<(std::ostream& stream, const FrameIndex &r){
		return stream << static_cast<const BaseRational>(r);
	}
};

struct Time: public BaseRational {
	Time(const value_type num = 0, const value_type den = 1) :
			BaseRational(num, den) {
	}
	Time(const BaseRational rational) :
			BaseRational(rational) {
	}
	Time(const std::chrono::microseconds value) :
			BaseRational(value.count(), std::micro::den) {
	}
	std::chrono::microseconds asMicroseconds() const {
		const value_type approx = value_type((double(numerator()) / denominator() * std::micro::den) + .5);
		return std::chrono::microseconds(approx);
	}
	double asDouble() const {
		return double(numerator()) / denominator();
	}
};

struct FrameDuration: public BaseRational {
	FrameDuration(value_type num, value_type den = 1) :
			BaseRational(num, den) {
		if (num == 0)
			throw std::domain_error("can't have a frame lasting zero seconds");
	}
	const static FrameDuration FILM;
	const static FrameDuration PAL;
	const static FrameDuration NTSC;
};

Time frameToTime(const uint32_t frame, const FrameDuration &period);
FrameIndex timeToFrame(Time time, const FrameDuration &period);

#endif /* FRAMEUTILS_H_ */
