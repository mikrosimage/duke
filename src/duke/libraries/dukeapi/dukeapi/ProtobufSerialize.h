/*
 * ProtobufSerialize.h
 *
 *  Created on: 22 sept. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef PROTOBUFSERIALIZE_H_
#define PROTOBUFSERIALIZE_H_

#include "protocol.pb.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/descriptor.h>
#include <boost/shared_ptr.hpp>

#include <iostream>

namespace google {

namespace protobuf {

namespace serialize {

inline const google::protobuf::Descriptor* descriptorFor(const MessageHolder &holder) {
    using namespace google::protobuf;
    assert(holder.has_type_name());
    const Descriptor* pDescriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(holder.type_name());
    if (!pDescriptor) {
        cerr << "No descriptor for message of type " << holder.type_name() << endl;
        return NULL;
    }
    return pDescriptor;
}

template<typename T>
inline bool isType(const google::protobuf::Descriptor* descriptor) {
    return descriptor == T::descriptor();
}

template<typename T>
inline bool isType(const MessageHolder &holder) {
    return descriptorFor(holder) == T::descriptor();
}

inline google::protobuf::Message* newInstanceFor(const google::protobuf::Descriptor* pDescriptor) {
    using namespace google::protobuf;
    if (!pDescriptor)
        return NULL;
    const google::protobuf::Message* pPrototype = MessageFactory::generated_factory()->GetPrototype(pDescriptor);
    return pPrototype ? pPrototype->New() : NULL;
}

inline void pack(MessageHolder &holder, const google::protobuf::Message &msg, MessageHolder_Action action = MessageHolder::CREATE) {
    holder.Clear();
    holder.set_action(action);
    holder.set_type_name(msg.GetDescriptor()->full_name());
    msg.SerializeToString(holder.mutable_body());
}

typedef boost::shared_ptr<google::protobuf::serialize::MessageHolder> SharedHolder;
typedef boost::shared_ptr<google::protobuf::Message> SharedMessage;
inline SharedMessage unpack(const MessageHolder &holder) {
    using namespace google::protobuf;
    using namespace std;
    const string &type_name(holder.type_name());
    const Descriptor *pDescriptor = descriptorFor(holder);
    if (!pDescriptor) {
        cerr << "Unable to find descriptor for class name " << type_name << endl;
        return SharedMessage();
    }
    SharedMessage pNew(newInstanceFor(pDescriptor));
    if (!pNew) {
        cerr << "Unable to find a prototype for descriptor " << pDescriptor->DebugString() << endl;
        return SharedMessage();
    }
    if (!pNew->ParseFromString(holder.body())) {
        cerr << "Unable to unmarshal MessageHolder's body into a " << type_name << ". Message looks corrupted." << endl;
        return SharedMessage();
    }
    return pNew;
}

inline bool unpack(const MessageHolder &holder, google::protobuf::Message& message) {
    const string &type_name(holder.type_name());
    const Descriptor *pDescriptor = descriptorFor(holder);
    if (!pDescriptor) {
        cerr << "Unable to find descriptor for class name " << type_name << endl;
        return false;
    }
    if (pDescriptor != message.GetDescriptor()) {
        cerr << "Expected message of type " << pDescriptor->name() << " but was " << message.GetDescriptor()->name() << endl;
        return false;
    }
    if (!message.ParseFromString(holder.body())) {
        cerr << "Unable to unmarshal MessageHolder's body into a " << type_name << ". Message looks corrupted." << endl;
        return false;
    }
    return true;
}

template<typename Dst>
Dst unpackTo(const MessageHolder &holder) {
    Dst object;
    unpack(holder, object);
    return object;
}

inline SharedHolder makeSharedHolder(const MessageHolder &holder) {
    return SharedHolder(new MessageHolder(holder));
}

inline SharedHolder packAndShare(const ::google::protobuf::Message &msg, MessageHolder_Action action = MessageHolder::CREATE) {
    MessageHolder tmp;
    pack(tmp, msg, action);
    return makeSharedHolder(tmp);
}

inline bool writeDelimitedTo(ostream &s, const Message &msg) {
    using namespace google::protobuf::io;
    OstreamOutputStream oos(&s);
    CodedOutputStream cos(&oos);
    cos.WriteVarint32(msg.ByteSize());
    return msg.SerializeToCodedStream(&cos);
}

inline bool writeDelimitedAndFlushTo(ostream &s, const Message &msg) {
    const bool ok = writeDelimitedTo(s, msg);
    s.flush();
    return ok;
}

inline bool parseDelimitedFrom(istream &s, Message &msg) {
    using namespace google::protobuf::io;
    msg.Clear();
    uint32_t size;
    {
        IstreamInputStream iis(&s, 1);
        CodedInputStream cis(&iis);
        if (!cis.ReadVarint32(&size))
            return false;
    }
    {
        IstreamInputStream iis(&s, size);
        CodedInputStream cis(&iis);
        const CodedInputStream::Limit limit = cis.PushLimit(size);
        if (!msg.ParseFromCodedStream(&cis))
            return false;
        cis.PopLimit(limit);
        return true;
    }
}

typedef std::vector<uint8_t> raw_buffer;
inline void writeDelimitedTo(raw_buffer &buffer, const MessageHolder &holder) {
    using google::protobuf::io::CodedOutputStream;
    const uint32_t msgSize = holder.ByteSize();
    buffer.resize(msgSize + CodedOutputStream::VarintSize32(msgSize));
    holder.SerializeToArray(CodedOutputStream::WriteVarint32ToArray(msgSize, buffer.data()), msgSize);
}

inline void readDelimitedFrom(const raw_buffer &buffer, MessageHolder &holder) {
    using google::protobuf::io::CodedInputStream;
    CodedInputStream stream(buffer.data(), buffer.size());
    uint32_t msgSize = 0;
    stream.ReadVarint32(&msgSize);
}

} // namespace serialize

} // namespace protobuf

} // namespace google

#endif /* PROTOBUFSERIALIZE_H_ */
