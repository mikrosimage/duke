#ifndef AVERAGESYNCMETHOD_H
#define AVERAGESYNCMETHOD_H

#include "ISyncMethod.h"
#include <boost/foreach.hpp>
#include <iostream>
#include <deque>
#include <math.h>

#define _SYNC_TICK_TIME_MS_ 200
#define _SYNC_AVERAGE_STEPS_ 5

class AverageSyncMethod : public ISyncMethod {

public:
    typedef boost::shared_ptr<AverageSyncMethod> ptr;

public:
    void wait() {
        boost::this_thread::sleep(boost::posix_time::millisec(_SYNC_TICK_TIME_MS_));
    }

    bool compute(float & _nextPitch) {
        if (m_LastPitchValues.size() > _SYNC_AVERAGE_STEPS_ - 1)
            m_LastPitchValues.pop_front();

        float p = pow(2,(m_VideoOffset - m_SoundOffset) / (_SYNC_TICK_TIME_MS_ / 1000.f));

        // compute average of N last pitch values
        BOOST_FOREACH(float f, m_LastPitchValues){
            p += f;
        }
        _nextPitch = p / (m_LastPitchValues.size() + 1);
        m_LastPitchValues.push_back(_nextPitch);

        return true;
    }

private:
    std::deque<float> m_LastPitchValues;
};

#endif // AVERAGESYNCMETHOD_H
