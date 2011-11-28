/*
 * BlockingAccessor.hpp
 *
 *  Created on: 22 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef BLOCKINGACCESSOR_HPP_
#define BLOCKINGACCESSOR_HPP_

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>

#include <cassert>
#include <stdexcept>
#include <atomic>

struct terminated : public std::exception {
};

template<typename T>
struct BlockingAccessor : private boost::noncopyable {
    BlockingAccessor(const T&object) :
            m_Terminate(false) {
        internal_set(object);
    }

    void set(const T& object) {
        // locking the shared object
        ::boost::mutex::scoped_lock lock(m_Mutex);
        internal_set(object);
        lock.unlock();
        // notifying shared structure is updated
        m_Condition.notify_one();
    }

    void terminate() {
        m_Terminate = true;
        m_Condition.notify_all();
    }

    void waitGet(T& value) {
        ::boost::unique_lock<boost::mutex> lock(m_Mutex);

        checkTermination();

        // blocking until at least one element in the list
        while (!m_SharedObjectSet) {
            m_Condition.wait(lock);
            checkTermination();
        }

        internal_unset(value);
    }

    bool tryGet(T& holder) {
        // locking the shared object
        ::boost::lock_guard<boost::mutex> lock(m_Mutex);
        checkTermination();

        if (!m_SharedObjectSet)
            return false;

        internal_unset(holder);
        return true;
    }
private:
    inline void checkTermination() const {
        if (m_Terminate)
            throw terminated();
    }

    inline void internal_set(const T& value){
        m_SharedObject = value;
        m_SharedObjectSet = true;
    }

    inline void internal_unset(T& value){
        assert(m_SharedObjectSet);
        value = m_SharedObject;
        m_SharedObjectSet = false;
    }

    ::boost::mutex m_Mutex;
    ::boost::condition_variable m_Condition;
    std::atomic<bool> m_Terminate;
    T m_SharedObject;
    bool m_SharedObjectSet;
};

#endif /* BLOCKINGACCESSOR_HPP_ */
