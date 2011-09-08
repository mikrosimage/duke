#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <dukeapi/core/IMessageIO.h>
#include <dukeapi/core/MessageHolder.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/noncopyable.hpp>
#include <list>

class MessageQueue : public IMessageIO, public boost::noncopyable {
public:
    virtual void push(const SharedMessage& holder);
    virtual void waitPop( SharedMessage& holder );
    virtual bool tryPop( SharedMessage& holder );

protected:
    std::list<SharedMessage> m_MessageList;
    ::boost::condition_variable m_Condition;
    ::boost::mutex m_Mutex;
};

#endif /* MESSAGEQUEUE_H_ */
