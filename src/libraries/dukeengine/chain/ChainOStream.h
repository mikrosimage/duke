/*
 * ChainOStream.h
 *
 *  Created on: 10 nov. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef CHAINOSTREAM_H_
#define CHAINOSTREAM_H_

#include "Chain.h"

std::ostream& operator<<(std::ostream& stream, const Slot::State& state);
std::ostream& operator<<(std::ostream& stream, const Slot& slot);
std::ostream& operator<<(std::ostream& stream, const TChain& chain);
std::ostream& operator<<(std::ostream& stream, const Chain& chain);

#endif /* CHAINOSTREAM_H_ */
