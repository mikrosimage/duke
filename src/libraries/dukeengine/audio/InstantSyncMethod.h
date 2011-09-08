#ifndef INSTANTSYNCMETHOD_H
#define INSTANTSYNCMETHOD_H

#include "ISyncMethod.h"
#include <iostream>
#include <math.h>

#define _SYNC_TICK_TIME_MS_ 200

class InstantSyncMethod : public ISyncMethod {

public:
    typedef boost::shared_ptr<InstantSyncMethod> ptr;

public:
    void wait() {
        boost::this_thread::sleep(boost::posix_time::millisec(_SYNC_TICK_TIME_MS_));
    }

    bool compute(float & _nextPitch) {
        _nextPitch = (m_VideoOffset - m_SoundOffset) / (_SYNC_TICK_TIME_MS_/1000.f);
        _nextPitch = pow(2,_nextPitch);
        return true;
    }

};

#endif // INSTANTSYNCMETHOD_H
