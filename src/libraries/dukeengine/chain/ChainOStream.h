#ifndef CHAINOSTREAM_H_
#define CHAINOSTREAM_H_

#include "Chain.h"

std::ostream& operator<<(std::ostream& stream, const Slot::State& state);
std::ostream& operator<<(std::ostream& stream, const Slot& slot);
std::ostream& operator<<(std::ostream& stream, const TChain& chain);
std::ostream& operator<<(std::ostream& stream, const Chain& chain);

#endif /* CHAINOSTREAM_H_ */
