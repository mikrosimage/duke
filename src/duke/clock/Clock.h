/*
 * Clock.h
 *
 *  Created on: Nov 25, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include <chrono>
#include <deque>
#include <cstdint>

/**
 * Measures elapsed time
 */
struct StopWatch {
	StopWatch() :
			m_LastTime(std::chrono::steady_clock::now()) {
	}
	inline const std::chrono::microseconds elapsedMicroSeconds() {
		const auto now = std::chrono::steady_clock::now();
		const auto elapsed = now - m_LastTime;
		m_LastTime = now;
		return std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
	}
private:
	std::chrono::time_point<std::chrono::steady_clock> m_LastTime;
};

/**
 * Gathers statistic about tick events
 */
struct Metronom {
	Metronom(std::size_t bufferSize);
	void tick();
	void compute();
	double getMin() const {
		return m_Min;
	}
	double getMax() const {
		return m_Max;
	}
	double getMean() const {
		return m_Mean;
	}
	double getStdDev() const {
		return m_StdDev;
	}
	double getFPS() const {
		return 1000 / m_Mean;
	}
	void dump();
private:
	const std::size_t m_Size;
	std::deque<double> m_Durations;
	StopWatch m_StopWatch;
	double m_Min, m_Max, m_Mean, m_StdDev;
};

#endif /* CLOCK_H_ */
