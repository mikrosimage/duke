#include "Clock.hpp"

#include <numeric>
#include <algorithm>
#include <cmath>

Metronom::Metronom(size_t values) :
		m_Size(values), m_Min(0), m_Max(0), m_Mean(0), m_StdDev(0) {
}

std::chrono::microseconds Metronom::tick() {
	if (m_Durations.size() >= m_Size)
		m_Durations.pop_front();
	const auto elapsedMicroSeconds = m_StopWatch.elapsedMicroSeconds();
	const double elapsedMilliSeconds = elapsedMicroSeconds.count() / 1000.;
	m_Durations.push_back(elapsedMilliSeconds);
	return elapsedMicroSeconds;
}

void Metronom::compute() {
	if(m_Durations.empty())
		return;
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
	printf("tick : min %2.2fms, max %2.2fms, mean %2.2fms, stddev %2.3fms, fps=%f\n", m_Min, m_Max, m_Mean, m_StdDev, getFPS());
}
