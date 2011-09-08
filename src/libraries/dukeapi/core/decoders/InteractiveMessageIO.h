#ifndef INTERACTIVEDECODER_H_
#define INTERACTIVEDECODER_H_

#include <dukeapi/core/IMessageIO.h>
#include <dukeapi/core/queue/MessageQueue.h>

class InteractiveMessageIO : public IMessageIO {
public:
    InteractiveMessageIO(MessageQueue& initialMessages);
    virtual ~InteractiveMessageIO();

    virtual void push(const SharedMessage& holder);
    virtual void waitPop(SharedMessage& holder);
    virtual bool tryPop(SharedMessage& holder);

private:
    MessageQueue &m_ToApplicationQueue;
    bool m_bPlay;
    unsigned int m_iFitMode;
    std::stringstream m_ssSeek;
};

#endif /* INTERACTIVEDECODER_H_ */
