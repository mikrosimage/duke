/*
 * IMessageIO.h
 *
 *  Created on: 31 août 2010
 *      Author: Guillaume Chatelet
 */

#ifndef IMESSAGEIO_H_
#define IMESSAGEIO_H_

#include "MessageHolder.h"

struct IMessageIO {
    virtual void push(const SharedMessage& holder) = 0;
    virtual void waitPop(SharedMessage& holder) = 0;
    virtual bool tryPop(SharedMessage& holder) = 0;
};

void push(IMessageIO&, const google::protobuf::Message&, //
          ::protocol::duke::MessageType_Action action = ::protocol::duke::MessageType_Action_SET);

void push(IMessageIO&, std::unique_ptr<google::protobuf::Message>&, //
          ::protocol::duke::MessageType_Action action = ::protocol::duke::MessageType_Action_SET);

#endif /* IMESSAGEIO_H_ */
