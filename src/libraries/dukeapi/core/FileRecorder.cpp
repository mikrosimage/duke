#include "FileRecorder.h"
#include <player.pb.h>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace ::google::protobuf::serialize;

FileRecorder::FileRecorder(const char* filename, IMessageIO& delegate) :
    m_Delegate(delegate), m_Stream(filename, std::ifstream::out | std::ifstream::binary) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    if (!m_Stream.is_open())
        throw std::runtime_error("Unable to open script file " + string(filename));
}

FileRecorder::~FileRecorder() {
}

void FileRecorder::push(const SharedHolder& holder) {
    m_Delegate.push(holder);
}

bool FileRecorder::tryPop(SharedHolder& holder) {
    const bool succeeded = m_Delegate.tryPop(holder);
    if (holder)
        writeDelimitedAndFlushTo(m_Stream, *holder);
    return succeeded;
}

void FileRecorder::waitPop(SharedHolder& holder) {
    m_Delegate.waitPop(holder);
    if (holder)
        writeDelimitedAndFlushTo(m_Stream, *holder);
}
