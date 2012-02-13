#ifndef SHADINGGRAPHCODEBUILDER_H_
#define SHADINGGRAPHCODEBUILDER_H_

#include "PrototypeFactory.h"

#include <string>

namespace shader_factory {

std::string compile(const ::duke::protocol::Program &program, const PrototypeFactory& factory);

} // namespace shader_factory

#endif /* SHADINGGRAPHCODEBUILDER_H_ */
