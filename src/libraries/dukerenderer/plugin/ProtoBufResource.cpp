/*
 * ProtoBufResource.cpp
 *
 *  Created on: 12 juil. 2010
 *      Author: Guillaume Chatelet
 */

#include "ProtoBufResource.h"
#include <google/protobuf/message.h>

ProtoBufResource::ProtoBufResource( const ::google::protobuf::Message& message ) : m_pMessage( message.New() )
{
	m_pMessage->CopyFrom( message );
}

ProtoBufResource::~ProtoBufResource() {}

