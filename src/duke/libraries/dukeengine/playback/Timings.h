#ifndef DISPLAYTIMING_H_
#define DISPLAYTIMING_H_

#include <string>
#include <boost/chrono.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/rolling_count.hpp>

enum TimingType {
    FRAME, VBI
};

class Timings {
    typedef boost::chrono::high_resolution_clock Clock;
    typedef Clock::time_point TimePoint;
    typedef Clock::duration Duration;

public:
    Timings(const TimingType type, const size_t samples);

    void push(const TimePoint&);

    inline double usPerSample() const {
        return boost::accumulators::rolling_mean(m_Accumulator);
    }
    inline double frequency() const {
        return 1000000 / usPerSample();
    }
    inline TimePoint lastSample() const {
        return m_LastSample;
    }

private:
    inline bool isDurationAvailable() const {
        return m_LastSample.time_since_epoch().count() != 0;
    }
    inline bool isMeanAvailable() const {
        return boost::accumulators::rolling_count(m_Accumulator) > 0;
    }
    inline long long int elapsedTime(const TimePoint &now) const {
        return boost::chrono::duration_cast<boost::chrono::microseconds>(now - m_LastSample).count();
    }

    void handleVBI(const long long int);
    void handleFrame(const long long int);

    const TimingType m_Type;
    boost::accumulators::accumulator_set< //
            long long int, //
            boost::accumulators::stats<boost::accumulators::tag::rolling_mean>, //
            boost::accumulators::stats<boost::accumulators::tag::rolling_count> //
    > m_Accumulator;
    TimePoint m_LastSample;
};

#endif /* DISPLAYTIMING_H_ */
