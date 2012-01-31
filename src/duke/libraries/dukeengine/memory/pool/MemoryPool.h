#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#include "IMemoryPool.h"
#include <climits>
#include <list>
#include <sstream>
#include <numeric>
#include <functional>

class PoolData; ///< forward declaration
class IPool
{
public:
	virtual void referenced( PoolData* ) = 0;
	virtual void released( PoolData* )   = 0;
};

class MemoryPool : public IMemoryPool,
	public IPool
{
public:
	MemoryPool( const size_t maxSize = ULONG_MAX );
	~MemoryPool();

	IDataPtr allocate( const size_t size ) throw( std::bad_alloc, std::length_error );

	void referenced( PoolData* );
	void released( PoolData* );

	size_t getUsedMemorySize() const;
	size_t getAllocatedMemorySize() const;
	size_t getMaxMemorySize() const;
	size_t getAvailableMemorySize() const;
	size_t getWastedMemorySize() const;

	void clear( size_t size );
	void clearOne();
	void clearAll();

private:
	typedef std::list<PoolData*> DataList;
	DataList m_DataUsed;
	DataList m_DataUnused;
	const size_t m_uMemoryAuthorized;
};

#endif /* MEMORYPOOL_H_ */

