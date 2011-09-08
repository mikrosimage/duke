/*
 * ProtoBufResource.h
 *
 *  Created on: 12 juil. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef PROTOBUFRESOURCE_H_
#define PROTOBUFRESOURCE_H_

#include "IResource.h"
#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>
#include <cassert>

class ProtoBufResource : public IResource
{
public: ProtoBufResource( const ::google::protobuf::Message& message );
	virtual ~ProtoBufResource();

	template<class MESSAGE_TYPE>
	const inline MESSAGE_TYPE* get() const
	{
		assert( m_pMessage );
		return dynamic_cast<MESSAGE_TYPE*>( m_pMessage.get() );
	}

	template<class MESSAGE_TYPE>
	const inline MESSAGE_TYPE& getRef() const
	{
		return *get<MESSAGE_TYPE>();
	}

	virtual const ::resource::Type getResourceType() const { return ::resource::PROTOBUF; }
	virtual std::size_t getSize() const { return m_pMessage->ByteSize(); }

private:
	const boost::shared_ptr<google::protobuf::Message> m_pMessage;
};

typedef boost::shared_ptr<ProtoBufResource> TResourcePtr;

#endif /* PROTOBUFRESOURCE_H_ */
