#ifndef DKXOBSERVER_H
#define DKXOBSERVER_H

#include <dukeapi/ProtobufSerialize.h>
#include <boost/shared_ptr.hpp>

class IObserver {
public:
    virtual void update(::google::protobuf::serialize::SharedHolder holder) = 0;
};

#endif // DKXOBSERVER_H
