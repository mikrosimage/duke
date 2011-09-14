#include "IMessageIO.h"

using namespace google::protobuf;
using namespace duke::protocol;

void push(IMessageIO& io, const Message& msg, MessageType_Action action) {
    io.push(SharedMessage(new MessageCopy(msg, action)));
}

void push(IMessageIO& io, std::unique_ptr<Message>& msg, MessageType_Action action) {
    io.push(SharedMessage(new MessageCopy(msg, action)));
}
