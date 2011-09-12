#include "MessageQueue.h"

void MessageQueue::push(const SharedMessage& holder) {
    // locking the shared object
    ::boost::lock_guard<boost::mutex> lock(m_Mutex);
    m_MessageList.push_back(holder);
    // notifying shared structure is updated
    m_Condition.notify_one();
}

void MessageQueue::waitPop(SharedMessage& holder) {
    // blocking until at least one element in the list
    ::boost::unique_lock<boost::mutex> lock(m_Mutex);

    while (m_MessageList.empty())
        m_Condition.wait(lock);

    // popping actually
    assert( !m_MessageList.empty() );
    holder = m_MessageList.front();
    m_MessageList.pop_front();
}

bool MessageQueue::tryPop(SharedMessage& holder) {
    // locking the shared object
    ::boost::lock_guard<boost::mutex> lock(m_Mutex);

    if (m_MessageList.empty()){
        holder.reset();
        return false;
    }

    assert( !m_MessageList.empty() );
    holder = m_MessageList.front();
    // popping actually
    m_MessageList.pop_front();
    return holder.get() != NULL;
}
