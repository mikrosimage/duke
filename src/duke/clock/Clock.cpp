/*
 * Clock.cpp
 *
 *  Created on: Nov 25, 2012
 *      Author: Guillaume Chatelet
 */

#include "Clock.h"

#include <numeric>
#include <algorithm>

Metronom::Metronom(size_t values) :
		m_Size(values), m_Min(0), m_Max(0), m_Mean(0), m_StdDev(0) {
}

void Metronom::tick() {
	if (m_Durations.size() >= m_Size)
		m_Durations.pop_front();
	m_Durations.push_back(m_StopWatch.elapsedMicroSeconds().count() / 1000.);
}

void Metronom::compute() {
	const auto min_max = std::minmax_element(m_Durations.begin(), m_Durations.end());
	m_Min = *min_max.first;
	m_Max = *min_max.second;
	const auto sum = std::accumulate(m_Durations.begin(), m_Durations.end(), 0.);
	m_Mean = sum / m_Durations.size();
	double stddev = 0;
	for (const auto duration : m_Durations)
		stddev += (duration - m_Mean) * (duration - m_Mean);
	m_StdDev = sqrt(stddev / m_Durations.size());
}

void Metronom::dump() {
	compute();
	printf("tick : min %2.2lfms, max %2.2lfms, mean %2.2lfms, stddev %2.3lfms, fps=%lf\n", m_Min, m_Max, m_Mean, m_StdDev, getFPS());
}
