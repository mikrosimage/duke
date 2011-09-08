#ifndef ISYNCMETHOD_H
#define ISYNCMETHOD_H

#include <boost/shared_ptr.hpp>

class ISyncMethod {

public:
    typedef boost::shared_ptr<ISyncMethod> ptr;
    virtual ~ISyncMethod() {
    }

public:
    virtual void init(const float & _videoOffset, const float & _soundOffset, const float & _videoFramerate) {
        m_VideoOffset = _videoOffset;
        m_SoundOffset = _soundOffset;
        m_VideoFramerate = _videoFramerate;
    }

    virtual void wait() {
    }

    virtual bool compute(float & _nextPitch) = 0;

protected:
    float m_VideoOffset;
    float m_SoundOffset;
    float m_VideoFramerate;
};

#endif // ISYNCMETHOD_H
