#include "PlaybackReader.h"
#include <player.pb.h>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::serialize;
using namespace duke::protocol;

PlaybackReader::PlaybackReader(const char* filename) :
    m_Stream(filename, std::ifstream::in | std::ifstream::binary) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    if (!m_Stream.is_open())
        throw std::runtime_error("Unable to open script file " + string(filename));
}

PlaybackReader::~PlaybackReader() {
    m_Stream.close();
}

void PlaybackReader::push(const SharedHolder& holder) {
}

void PlaybackReader::waitPop(SharedHolder& holder) {
    MessageHolder tmp;
    if (parseDelimitedFrom(m_Stream, tmp))
        holder = makeSharedHolder(tmp);
}

bool PlaybackReader::tryPop(SharedHolder& holder) {
    MessageHolder tmp;
    if (!parseDelimitedFrom(m_Stream, tmp)) {
        holder.reset();
        return false;
    }
    return true;
}
