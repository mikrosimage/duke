#ifndef CONCURRENTQUEUE_H_
#define CONCURRENTQUEUE_H_

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <queue>
#include <list>
#include <cassert>
#include <algorithm>

template<typename T, typename Queue = std::queue<T> >
struct ConcurrentQueue {
public:
	typedef Queue queue_type;

	void push(const T& holder) {
		// locking the shared object
		::boost::lock_guard<boost::mutex> lock(m_Mutex);
		m_Queue.push(holder);
		// notifying shared structure is updated
		m_Condition.notify_one();
	}

	void drainFrom(Queue &collection) {
		if (collection.empty())
			return;
		// locking the shared object
		::boost::lock_guard<boost::mutex> lock(m_Mutex);
		drain(collection, m_Queue);

		// notifying shared structure is updated
		m_Condition.notify_one();
	}

	void waitPop(T& holder) {
		::boost::unique_lock<boost::mutex> lock(m_Mutex);

		// blocking until at least one element in the list
		while (m_Queue.empty())
			m_Condition.wait(lock);

		// popping actually
		assert(!m_Queue.empty());
		holder = m_Queue.front();
		m_Queue.pop();
	}

	bool tryPop(T& holder) {
		// locking the shared object
		::boost::lock_guard<boost::mutex> lock(m_Mutex);

		if (m_Queue.empty())
			return false;

		assert(!m_Queue.empty());
		holder = m_Queue.front();
		m_Queue.pop();

		return true;
	}

	bool drainTo(Queue& collection) {
		// locking the shared object
		::boost::lock_guard<boost::mutex> lock(m_Mutex);

		if (m_Queue.empty())
			return false;

		assert(!m_Queue.empty());
		drain(m_Queue, collection);

		return true;
	}

protected:
	::boost::mutex m_Mutex;
	::boost::condition_variable m_Condition;
	Queue m_Queue;
private:
	static void drain(Queue& from, Queue& to) {
		while (!from.empty()) {
			to.push(from.front());
			from.pop();
		}
	}
};

#endif /* CONCURRENTQUEUE_H_ */
