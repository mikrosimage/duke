#ifndef CHAINOSTREAM_H_
#define CHAINOSTREAM_H_

#include "Chain.h"

std::ostream& operator<<(std::ostream& stream, const State& state);
std::ostream& operator<<(std::ostream& stream, const InternalSlot& slot);
std::ostream& operator<<(std::ostream& stream, const TChain& chain);
std::ostream& operator<<(std::ostream& stream, const Chain& chain);

#endif /* CHAINOSTREAM_H_ */
