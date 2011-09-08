/*
 * IMessageOutput.cpp
 *
 *  Created on: 15 juin 2011
 *      Author: Guillaume Chatelet
 */

#include "IMessageIO.h"

using namespace google::protobuf;
using namespace protocol::duke;

void push(IMessageIO& io, const Message& msg, MessageType_Action action) {
    io.push(SharedMessage(new MessageCopy(msg, action)));
}

void push(IMessageIO& io, std::unique_ptr<Message>& msg, MessageType_Action action) {
    io.push(SharedMessage(new MessageCopy(msg, action)));
}
