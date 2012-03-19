#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include "IMessageIO.h"
#include <concurrent/ConcurrentQueue.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <queue>

struct MessageQueue : public IMessageIO, private concurrent::ConcurrentQueue<google::protobuf::serialize::SharedHolder> {
private:
    typedef concurrent::ConcurrentQueue<google::protobuf::serialize::SharedHolder> UP;

public:
    using UP::drainTo;
    using UP::drainFrom;
    using UP::clear;

    virtual void push(const google::protobuf::serialize::SharedHolder& holder) {
        UP::push(holder);
    }

    virtual void waitPop(google::protobuf::serialize::SharedHolder& holder) {
        UP::waitPop(holder);
    }

    virtual bool tryPop(google::protobuf::serialize::SharedHolder& holder) {
        const bool popped = UP::tryPop(holder);
        if (!popped)
            holder.reset();
        return holder.get() != NULL;
    }
};

#endif /* MESSAGEQUEUE_H_ */
