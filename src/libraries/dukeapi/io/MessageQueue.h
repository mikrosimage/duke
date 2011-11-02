#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include "IMessageIO.h"
#include "ConcurrentQueue.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/noncopyable.hpp>
#include <queue>

struct MessageQueue : public IMessageIO, public boost::noncopyable, private ConcurrentQueue<google::protobuf::serialize::SharedHolder> {
private:
    typedef ConcurrentQueue<google::protobuf::serialize::SharedHolder> IMPL;
public:
    typedef IMPL::queue_type queue_type;

    virtual void push(const google::protobuf::serialize::SharedHolder& holder) {
        IMPL::push(holder);
    }

    virtual void waitPop(google::protobuf::serialize::SharedHolder& holder) {
        IMPL::waitPop(holder);
    }

    virtual bool tryPop(google::protobuf::serialize::SharedHolder& holder) {
        const bool popped = IMPL::tryPop(holder);
        if (!popped)
            holder.reset();
        return holder.get() != NULL;
    }

    void drainFrom(queue_type& collection) {
        IMPL::drainFrom(collection);
    }

    bool drainTo(queue_type& collection) {
        return IMPL::drainTo(collection);
    }
};

#endif /* MESSAGEQUEUE_H_ */
