#include "MessageQueue.h"

using namespace google::protobuf::serialize;

void MessageQueue::push(const SharedHolder& holder) {
    // locking the shared object
    ::boost::lock_guard<boost::mutex> lock(m_Mutex);
    m_Queue.push(holder);
    // notifying shared structure is updated
    m_Condition.notify_one();
}

void MessageQueue::waitPop(SharedHolder& holder) {
    // blocking until at least one element in the list
    ::boost::unique_lock<boost::mutex> lock(m_Mutex);

    while (m_Queue.empty())
        m_Condition.wait(lock);

    // popping actually
    assert( !m_Queue.empty() );
    holder = m_Queue.front();
    m_Queue.pop();
}

bool MessageQueue::tryPop(SharedHolder& holder) {
    // locking the shared object
    ::boost::lock_guard<boost::mutex> lock(m_Mutex);

    if (m_Queue.empty()) {
        holder.reset();
        return false;
    }

    assert( !m_Queue.empty() );
    holder = m_Queue.front();
    // popping actually
    m_Queue.pop();
    return holder.get() != NULL;
}
