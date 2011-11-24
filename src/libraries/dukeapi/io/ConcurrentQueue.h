#ifndef CONCURRENTQUEUE_H_
#define CONCURRENTQUEUE_H_

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <deque>
#include <cassert>

template<typename T, typename Container = std::deque<T> >
struct ConcurrentQueue : private boost::noncopyable {
public:
    typedef Container container_type;
    typedef typename Container::value_type value_type;
    typedef typename Container::const_reference const_reference;

    void push(const T& holder) {
        // locking the shared object
        boost::mutex::scoped_lock lock(m_Mutex);
        m_Container.push_back(holder);
        lock.unlock();
        // notifying shared structure is updated
        m_Condition.notify_one();
    }

    template<typename CompatibleContainer>
    void drainFrom(CompatibleContainer &collection) {
        if (collection.empty())
            return;
        // locking the shared object
        boost::mutex::scoped_lock lock(m_Mutex);
        drain<CompatibleContainer, Container>(collection, m_Container);

        // notifying shared structure is updated
        m_Condition.notify_one();
    }

    void waitPop(T& holder) {
        boost::mutex::scoped_lock lock(m_Mutex);

        // blocking until at least one element in the list
        while (m_Container.empty())
            m_Condition.wait(lock);

        // popping actually
        assert(!m_Container.empty());
        holder = m_Container.front();
        m_Container.pop_front();
    }

    bool tryPop(T& holder) {
        // locking the shared object
        boost::mutex::scoped_lock lock(m_Mutex);

        if (m_Container.empty())
            return false;

        assert(!m_Container.empty());
        holder = m_Container.front();
        m_Container.pop_front();

        return true;
    }

    template<typename CompatibleContainer>
    bool drainTo(CompatibleContainer& collection) {
        // locking the shared object
        boost::mutex::scoped_lock lock(m_Mutex);

        if (m_Container.empty())
            return false;

        assert(!m_Container.empty());
        drain<Container, CompatibleContainer>(m_Container, collection);

        return true;
    }

    void clear() {
        // locking the shared object
        boost::mutex::scoped_lock lock(m_Mutex);
        m_Container.clear();
    }
protected:
    ::boost::mutex m_Mutex;
    ::boost::condition_variable m_Condition;
    container_type m_Container;
private:
    template<typename C1, typename C2>
    static void drain(C1& from, C2& to) {
        while (!from.empty()) {
            to.push_back(from.front());
            from.pop_front();
        }
    }
};

template<typename Queue>
struct QueueAdapter {
    typedef typename Queue::value_type value_type;
    typedef typename Queue::const_reference const_reference;
    QueueAdapter(Queue& q) :
            _q(q) {
    }
    void push_back(const_reference & t) {
        _q.push(t);
    }
private:
    Queue& _q;
};

#endif /* CONCURRENTQUEUE_H_ */
