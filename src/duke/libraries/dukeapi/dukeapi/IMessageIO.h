#ifndef IMESSAGEIO_H_
#define IMESSAGEIO_H_

#include <dukeapi/ProtobufSerialize.h>

struct IMessageIO {
    virtual void push(const google::protobuf::serialize::SharedHolder& holder) = 0;
    virtual void waitPop(google::protobuf::serialize::SharedHolder& holder) = 0;
    virtual bool tryPop(google::protobuf::serialize::SharedHolder& holder) = 0;
    virtual ~IMessageIO(){};
};

inline void push(IMessageIO &queue, const google::protobuf::Message& msg,
                 google::protobuf::serialize::MessageHolder_Action action = google::protobuf::serialize::MessageHolder::CREATE) {
    queue.push(::google::protobuf::serialize::packAndShare(msg, action));
}

#endif /* IMESSAGEIO_H_ */
