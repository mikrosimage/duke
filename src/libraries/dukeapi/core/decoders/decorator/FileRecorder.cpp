#include "FileRecorder.h"
#include <dukeapi/protocol/player/communication.pb.h>
#include <dukeapi/core/stream/Helpers.h>
#include <iostream>
#include <stdexcept>

using namespace std;

FileRecorder::FileRecorder(const char* filename, IMessageIO& delegate) :
    m_Delegate(delegate), m_Stream(filename, std::ifstream::out | std::ifstream::binary) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    if (!m_Stream.is_open())
        throw std::runtime_error("Unable to open script file " + string(filename));
}

FileRecorder::~FileRecorder() {
}

void FileRecorder::push(const SharedMessage& holder) {
    m_Delegate.push(holder);
}

bool FileRecorder::tryPop(SharedMessage& holder) {
    const bool succeeded = m_Delegate.tryPop(holder);
    if (holder)
        writeHolder(holder, m_Stream);
    return succeeded;
}

void FileRecorder::waitPop(SharedMessage& holder) {
    m_Delegate.waitPop(holder);
    if (holder)
        writeHolder(holder, m_Stream);
}
