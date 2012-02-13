#ifndef IMEMORYPOOL_H_
#define IMEMORYPOOL_H_

#include <cstddef>
#include <stdexcept>
#include <boost/smart_ptr/intrusive_ptr.hpp>

class IRefCounted
{
public:
	virtual void addRef()  = 0;
	virtual void release() = 0;
};

class IData : public IRefCounted
{
public:
	virtual operator     char *()             = 0;
	virtual operator     const char *() const = 0;
	virtual const size_t size() const         = 0;
	virtual const size_t reservedSize() const = 0;
};

void intrusive_ptr_add_ref( IData* pData );
void intrusive_ptr_release( IData* pData );

typedef ::boost::intrusive_ptr<IData> IDataPtr;

class IMemoryPool
{
public:
	virtual size_t   getUsedMemorySize() const                                                = 0;
	virtual size_t   getAllocatedMemorySize() const                                           = 0;
	virtual size_t   getAvailableMemorySize() const                                           = 0;
	virtual size_t   getWastedMemorySize() const                                              = 0;
	virtual size_t   getMaxMemorySize() const                                                 = 0;
	virtual void     clear( size_t size )                                                     = 0;
	virtual void     clearOne()                                                               = 0;
	virtual void     clearAll()                                                               = 0;
	virtual IDataPtr allocate( const size_t size ) throw( std::bad_alloc, std::length_error ) = 0;
};

#endif /* IMEMORYPOOL_H_ */
