/*
 * QueueMessageIO.h
 *
 *  Created on: 29 sept. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef QUEUEMESSAGEIO_H_
#define QUEUEMESSAGEIO_H_

#include "IMessageIO.h"

struct QueueMessageIO : public IMessageIO {
    virtual void push(const google::protobuf::serialize::SharedHolder& holder) {
        outputQueue.push(holder);
    }

    virtual void waitPop(google::protobuf::serialize::SharedHolder& holder) {
        inputQueue.waitPop(holder);
    }

    virtual bool tryPop(google::protobuf::serialize::SharedHolder& holder) {
        return inputQueue.tryPop(holder);
    }

    MessageQueue inputQueue;
    MessageQueue outputQueue;
};

#endif /* QUEUEMESSAGEIO_H_ */
