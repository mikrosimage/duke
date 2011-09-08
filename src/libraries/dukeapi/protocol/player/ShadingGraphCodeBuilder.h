/*
 * ShadingGraphCodeBuilder.h
 *
 *  Created on: 8 mars 2011
 *      Author: Guillaume Chatelet
 */

#ifndef SHADINGGRAPHCODEBUILDER_H_
#define SHADINGGRAPHCODEBUILDER_H_

#include "PrototypeFactory.h"

#include <string>

namespace shader_factory {

std::string compile(const ::protocol::shader_assembler::Program &program, const PrototypeFactory& factory);

} // namespace shader_factory

#endif /* SHADINGGRAPHCODEBUILDER_H_ */
