#include "ProtoBufResource.h"
#include <google/protobuf/message.h>

ProtoBufResource::ProtoBufResource( const ::google::protobuf::Message& message ) : m_pMessage( message.New() )
{
	m_pMessage->CopyFrom( message );
}

ProtoBufResource::~ProtoBufResource() {}

