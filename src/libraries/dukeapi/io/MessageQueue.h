#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include "IMessageIO.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/noncopyable.hpp>
#include <queue>

class MessageQueue : public IMessageIO, public boost::noncopyable {
public:
    virtual void push(const google::protobuf::serialize::SharedHolder& holder);
    virtual void waitPop(google::protobuf::serialize::SharedHolder& holder);
    virtual bool tryPop(google::protobuf::serialize::SharedHolder& holder);

protected:
    std::queue<google::protobuf::serialize::SharedHolder> m_Queue;
    ::boost::condition_variable m_Condition;
    ::boost::mutex m_Mutex;
};

#endif /* MESSAGEQUEUE_H_ */
