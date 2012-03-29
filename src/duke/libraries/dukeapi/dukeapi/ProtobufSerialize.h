/*
 * ProtobufSerialize.h
 *
 *  Created on: 22 sept. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef PROTOBUFSERIALIZE_H_
#define PROTOBUFSERIALIZE_H_

#include <protocol.pb.h>

#include <boost/shared_ptr.hpp>
#include <iostream>

namespace google {
namespace protobuf {
class Descriptor;
namespace serialize {

typedef boost::shared_ptr<google::protobuf::serialize::MessageHolder> SharedHolder;
typedef boost::shared_ptr<google::protobuf::Message> SharedMessage;

/**
 * Type related functions
 */
google::protobuf::Message* newInstanceFor(const google::protobuf::Descriptor* pDescriptor);
const google::protobuf::Descriptor* descriptorFor(const MessageHolder &holder);
template<typename T> inline bool isType(const google::protobuf::Descriptor* descriptor) { return descriptor == T::descriptor(); }
template<typename T> inline bool isType(const MessageHolder &holder) { return descriptorFor(holder) == T::descriptor(); }

SharedHolder make_shared(const MessageHolder &holder);

// packing
void pack(const google::protobuf::Message &msg, MessageHolder &holder, MessageHolder_Action action = MessageHolder::CREATE);
MessageHolder pack(const google::protobuf::Message &msg, MessageHolder_Action action = MessageHolder::CREATE);
SharedHolder packAndShare(const ::google::protobuf::Message &msg, MessageHolder_Action action = MessageHolder::CREATE);

// unpacking
bool unpack(const MessageHolder &holder, google::protobuf::Message& message);
SharedMessage unpack(const MessageHolder &holder);
template<typename Dst> Dst unpackTo(const MessageHolder &holder) { Dst object; unpack(holder, object); return object; }

/**
 * Serializing
 */
typedef std::vector<uint8_t> raw_buffer;
bool writeDelimitedTo(ostream &s, const Message &msg);
bool writeDelimitedAndFlushTo(ostream &s, const Message &msg);
void writeDelimitedTo(raw_buffer &buffer, const MessageHolder &holder);

void readDelimitedFrom(const raw_buffer &buffer, MessageHolder &holder);
bool parseDelimitedFrom(istream &s, Message &msg);

} // namespace serialize
} // namespace protobuf
} // namespace google

#endif /* PROTOBUFSERIALIZE_H_ */
