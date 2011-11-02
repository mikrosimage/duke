#include "ChainOStream.h"

using namespace std;

ostream& operator<<(ostream& stream, const State& state) {
    switch (state) {
        case NEW:
            stream << "_";
            break;
        case LOADING:
            stream << "l";
            break;
        case LOADED:
            stream << "L";
            break;
        case DECODING:
            stream << "d";
            break;
        case READY:
            stream << "D";
            break;
        default:
            stream << "X";
            break;
    }
    return stream;
}

ostream& operator<<(ostream& stream, const InternalSlot& slot) {
//    stream << '[' << slot.m_Shared.m_ImageHash << '|' << slot.m_State << ']';
    stream << slot.m_State;
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
