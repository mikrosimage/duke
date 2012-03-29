/*
 * ProtobufSerialize.cpp
 *
 *  Created on: 29 mars 2012
 *      Author: Guillaume Chatelet
 */

#include <dukeapi/ProtobufSerialize.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::io;

namespace google {
namespace protobuf {
namespace serialize {

const Descriptor* descriptorFor(const MessageHolder &holder) {
    assert(holder.has_type_name());
    const Descriptor* pDescriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(holder.type_name());
    if (!pDescriptor) {
        cerr << "No descriptor for message of type " << holder.type_name() << endl;
        return NULL;
    }
    return pDescriptor;
}

Message* newInstanceFor(const Descriptor* pDescriptor) {
    if (!pDescriptor)
        return NULL;
    const Message* pPrototype = MessageFactory::generated_factory()->GetPrototype(pDescriptor);
    return pPrototype ? pPrototype->New() : NULL;
}

void pack(const Message &msg, MessageHolder &holder, MessageHolder_Action action) {
    holder.Clear();
    holder.set_action(action);
    holder.set_type_name(msg.GetDescriptor()->full_name());
    msg.SerializeToString(holder.mutable_body());
}

MessageHolder pack(const Message &msg, MessageHolder_Action action) {
    MessageHolder holder;
    pack(msg, holder, action);
    return holder;
}

SharedMessage unpack(const MessageHolder &holder) {
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

bool unpack(const MessageHolder &holder, Message& message) {
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

SharedHolder make_shared(const MessageHolder &holder) {
    return SharedHolder(new MessageHolder(holder));
}

SharedHolder packAndShare(const Message &msg, MessageHolder_Action action) {
    MessageHolder tmp;
    pack(msg, tmp, action);
    return make_shared(tmp);
}

bool writeDelimitedTo(ostream &s, const Message &msg) {
    OstreamOutputStream oos(&s);
    CodedOutputStream cos(&oos);
    cos.WriteVarint32(msg.ByteSize());
    return msg.SerializeToCodedStream(&cos);
}

struct FlushGuard{
    FlushGuard(ostream &stream) : stream(stream){}
    ~FlushGuard(){stream.flush();}
    ostream &stream;
};

bool writeDelimitedAndFlushTo(ostream &s, const Message &msg) {
    FlushGuard guard(s);
    return writeDelimitedTo(s, msg);
}

bool parseDelimitedFrom(istream &s, Message &msg) {
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

void writeDelimitedTo(raw_buffer &buffer, const MessageHolder &holder) {
    const uint32_t msgSize = holder.ByteSize();
    buffer.resize(msgSize + CodedOutputStream::VarintSize32(msgSize));
    holder.SerializeToArray(CodedOutputStream::WriteVarint32ToArray(msgSize, buffer.data()), msgSize);
}

void readDelimitedFrom(const raw_buffer &buffer, MessageHolder &holder) {
    CodedInputStream stream(buffer.data(), buffer.size());
    uint32_t msgSize = 0;
    stream.ReadVarint32(&msgSize);
}

} // namespace serialize
} // namespace protobuf
} // namespace google
