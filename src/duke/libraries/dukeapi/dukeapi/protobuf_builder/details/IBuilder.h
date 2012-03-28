/*
 * IBuilder.h
 *
 *  Created on: 22 mars 2012
 *      Author: Guillaume Chatelet
 */

#ifndef IBUILDER_H_
#define IBUILDER_H_

#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>
#include <string>

class IBuilder {
public:
    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name) = 0;
    virtual ~IBuilder() {
    }
};

#endif /* IBUILDER_H_ */
