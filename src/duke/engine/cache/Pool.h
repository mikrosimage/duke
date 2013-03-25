/*
 * Pool.h
 *
 *  Created on: Feb 12, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <functional>
#include <memory>
#include <map>
#include <stack>

namespace pool {

template<typename KEY, typename DATA>
struct PoolBase {
	PoolBase() = default;
	PoolBase(const PoolBase&) = delete;
	PoolBase& operator=(const PoolBase&) = delete;

	typedef KEY key_type;
	typedef DATA value_type;

	typedef std::shared_ptr<value_type> DataPtr;
	typedef std::stack<DataPtr> DataStack;
	typedef std::map<key_type, DataStack> PoolMap;
};

template<class BASE>
struct Pool: public BASE {
	using typename BASE::DataPtr;
	using typename BASE::key_type;
	using typename BASE::value_type;

	DataPtr get(const key_type &key) {
		auto& stack = m_Pool[key];
		if (!stack.empty()) {
			DataPtr pData = std::move(stack.top());
			stack.pop();
			return pData;
		}
		return {BASE::evictAndCreate(key, m_Pool), recycleFunc()};
	}
private:
	void recycle(value_type* pData) {
		m_Pool[BASE::retrieveKey(pData)].emplace(pData, recycleFunc());
	}
	inline std::function<void(value_type*)> recycleFunc() {
		return std::bind(&Pool::recycle, this, std::placeholders::_1);
	}
	typename BASE::PoolMap m_Pool;
};

}  // namespace pool
