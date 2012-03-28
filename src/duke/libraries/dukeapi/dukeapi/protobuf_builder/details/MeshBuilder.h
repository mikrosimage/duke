/*
 * MeshBuilder.h
 *
 *  Created on: 22 mars 2012
 *      Author: Guillaume Chatelet
 */

#ifndef MESHBUILDER_H_
#define MESHBUILDER_H_

#include "IBuilder.h"
#include <player.pb.h>

#include <string>

namespace duke {
namespace protocol {

class MeshBuilder : public IBuilder {
public:
    typedef Mesh message_type;
    const static std::string plane;
    virtual boost::shared_ptr<google::protobuf::Message> build(const std::string name);
    static Mesh buildPlane(const std::string &name);
};

}  // namespace protocol
}  // namespace duke

#endif /* MESHBUILDER_H_ */
