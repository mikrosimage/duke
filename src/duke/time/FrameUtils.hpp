#pragma once

#include "duke/math/Rational.hpp"

#include <chrono>

typedef boost::rational<int64_t> BaseRational;

struct FrameIndex : public BaseRational {
  FrameIndex() : BaseRational() {}
  FrameIndex(const BaseRational rational) : BaseRational(rational) {}
  int_type round() const { return int_type((double(numerator()) / denominator())); }
  friend std::ostream& operator<<(std::ostream& stream, const FrameIndex& r) {
    return stream << static_cast<const BaseRational>(r);
  }
};

struct Time : public BaseRational {
  Time(const int_type num = 0, const int_type den = 1) : BaseRational(num, den) {}
  Time(const BaseRational rational) : BaseRational(rational) {}
  Time(const std::chrono::microseconds value) : BaseRational(value.count(), std::micro::den) {}
  int_type asMilliseconds() const { return int_type((double(numerator()) / denominator() * std::milli::den) + .5); }
  int_type asMicroseconds() const { return int_type((double(numerator()) / denominator() * std::micro::den) + .5); }
  double asDouble() const { return double(numerator()) / denominator(); }
  friend std::ostream& operator<<(std::ostream& stream, const Time& r) {
    return stream << static_cast<const BaseRational>(r);
  }
};

struct FrameDuration : public BaseRational {
  FrameDuration(int_type num, int_type den = 1) : BaseRational(num, den) {
    if (num == 0) throw std::domain_error("can't have a frame lasting zero seconds");
  }
  const static FrameDuration FILM;
  const static FrameDuration PAL;
  const static FrameDuration NTSC;
};

Time frameToTime(const uint32_t frame, const FrameDuration& period);
FrameIndex timeToFrame(Time time, const FrameDuration& period);
