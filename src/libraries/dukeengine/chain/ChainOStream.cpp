/*
 * ChainOStream.cpp
 *
 *  Created on: 10 nov. 2010
 *      Author: Guillaume Chatelet
 */

#include "ChainOStream.h"

using namespace std;

ostream& operator<<(ostream& stream, const Slot::State& state) {
    switch (state) {
        case Slot::NEW:
            stream << "_";
            break;
        case Slot::LOADING:
            stream << "l";
            break;
        case Slot::LOADED:
            stream << "L";
            break;
        case Slot::DECODING:
            stream << "d";
            break;
        case Slot::READY:
            stream << "D";
            break;
        default:
            stream << "X";
            break;
    }
    return stream;
}

ostream& operator<<(ostream& stream, const Slot& slot) {
    stream << '[' << slot.m_Shared.m_ImageHash << '|' << slot.m_State << ']';
    return stream;
}

ostream& operator<<(ostream& stream, const TChain& chain) {
    copy(chain.begin(), chain.end(), ostream_iterator<TChain::value_type> (stream, " "));
    return stream;
}

ostream& Chain::operator<<(ostream& stream) const {
    boost::mutex::scoped_lock lock(m_ChainMutex);
    stream << m_Chain;
    return stream;
}
ostream& operator<<(ostream& stream, const Chain& chain) {
    return chain.operator <<(stream);
}
