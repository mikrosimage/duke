#include "PlaybackReader.h"
#include <dukeapi/core/MessageHolder.h>
#include <dukeapi/core/stream/Helpers.h>
#include <dukeapi/protocol/player/communication.pb.h>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace google::protobuf;
using namespace protocol::duke;

PlaybackReader::PlaybackReader(const char* filename) :
    m_Stream(filename, std::ifstream::in | std::ifstream::binary) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    if (!m_Stream.is_open())
        throw std::runtime_error("Unable to open script file " + string(filename));
}

PlaybackReader::~PlaybackReader() {
    m_Stream.close();
}

void PlaybackReader::push(const SharedMessage& holder) {
}

void PlaybackReader::waitPop(SharedMessage& holder) {
    readHolder(holder, m_Stream);
}

bool PlaybackReader::tryPop(SharedMessage& holder) {
    switch (readHolder(holder, m_Stream)) {
        case IO_SUCCESS_OK:
            return true;
        default: {
            holder.reset();
            return false;
        }
    }
}
