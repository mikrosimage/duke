#ifndef INTERACTIVEDECODER_H_
#define INTERACTIVEDECODER_H_

#include <dukeapi/io/IMessageIO.h>
#include <dukeapi/io/MessageQueue.h>

class InteractiveMessageIO : public IMessageIO {
public:
    InteractiveMessageIO(MessageQueue& initialMessages);
    virtual ~InteractiveMessageIO();

    virtual void push(const ::google::protobuf::serialize::SharedHolder& holder);
    virtual void waitPop(::google::protobuf::serialize::SharedHolder& holder);
    virtual bool tryPop(::google::protobuf::serialize::SharedHolder& holder);

private:
    MessageQueue &m_ToApplicationQueue;
    bool m_bPlay;
    unsigned int m_iFitMode;
    float m_fGamma;
    float m_fExposure;
    std::stringstream m_ssSeek;
};

#endif /* INTERACTIVEDECODER_H_ */
