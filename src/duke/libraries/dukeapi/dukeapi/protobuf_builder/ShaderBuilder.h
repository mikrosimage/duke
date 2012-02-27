/*
 * ShaderBuilder.h
 *
 *  Created on: 24 févr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef SHADERBUILDER_H_
#define SHADERBUILDER_H_

#include <boost/noncopyable.hpp>
#include <string>
#include <vector>

struct IOQueueInserter;

namespace duke {
namespace protocol {

struct Clip;
struct Shader;

namespace shader_builder {

void pushCommonMessages(IOQueueInserter&);

void adapt( IOQueueInserter& inserter, Clip &clip, const std::string &extension);

} // namespace shader_builder


} /* namespace protocol */
} /* namespace duke */
#endif /* SHADERBUILDER_H_ */
