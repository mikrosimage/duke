#include "Timings.h"

#include <iostream>
#include <cmath>

using namespace boost::chrono;
using namespace boost::accumulators;
using namespace std;

Timings::Timings(const TimingType type, const size_t samples) :
    m_Type(type), m_Accumulator(tag::rolling_window::window_size = samples) {
}

void Timings::push(const boost::chrono::high_resolution_clock::time_point &now) {
    if (isDurationAvailable()) {
        const long long int us = elapsedTime(now);
        if (!isMeanAvailable())
            m_Accumulator(us);
        else {
            switch (m_Type) {
                case VBI:
                    handleVBI(us);
                    break;
                case FRAME:
                    handleFrame(us);
                    break;
            }
        }
    }
    m_LastSample = now;
}

void Timings::handleVBI(const long long int us) {
    const double mean = usPerSample();
    const long long int times = round(us / mean);
    if (times > 0)
        m_Accumulator(us / times);
}

void Timings::handleFrame(const long long int us) {
    m_Accumulator(us);
}
