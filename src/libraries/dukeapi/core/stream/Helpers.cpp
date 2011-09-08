#include "Helpers.h"

#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace ::std;
using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace ::protocol::duke;

const string HEADER = "[MessageIOHelper] ";
const string READHEADER = HEADER + "-read - ";
const string WRITEHEADER = HEADER + "-write- ";

#define CHECK(X) {check();check((X), #X);check();}

template<typename STREAM>
class Helper {
public:
    STREAM &stream;
    const string & header;

    Helper(STREAM &stream_, const string &header) :
        stream(stream_), header(header) {
    }

    inline void check() const {
        if (stream.eof())
            throw EIOState::IO_FAILED_EOF;
        if (stream.fail())
            throw EIOState::IO_FAILED_CORRUPTED_STREAM;
    }

    inline void check(bool result, const char* line) const {
        if (!result) {
            check(); // maybe we failed due to EOF
            cerr << header << '"' << line << '"' << " returned false, returning IO_CORRUPTED_STREAM" << endl;
            throw EIOState::IO_FAILED_CORRUPTED_STREAM;
        }
    }

    inline void checkInitialized(const Message &msg) const {
        if (!msg.IsInitialized()) {
            cerr << header << "google::protobuf::Message '" << msg.GetTypeName() << "' is not initialized, returning IO_BAD_MESSAGE" << endl;
            cerr << header << msg.DebugString() << endl;
            throw EIOState::IO_BAD_MESSAGE;
        }
    }
};

class ReadHelper : public Helper<istream> {
    void readMessageSize(uint32 &size) {
        IstreamInputStream byteStreamReader(&stream, 1);
        CHECK(CodedInputStream(&byteStreamReader).ReadVarint32(&size));
    }

    void readMessage(const uint32 &size, Message& msg) {
        IstreamInputStream streamReader(&stream, size);
        CodedInputStream codedInputStream(&streamReader);
        const CodedInputStream::Limit limit = codedInputStream.PushLimit(size);
        CHECK(msg.ParseFromCodedStream(&codedInputStream));
        codedInputStream.PopLimit(limit);
    }

    void readDelimitedMessage(Message& msg) {
        uint32 size(0);
        readMessageSize(size);
        if (size != 0)
            readMessage(size, msg);
        checkInitialized(msg);
    }

    void readHolder_(SharedMessage &holder) {
        // reading type
        MessageType type;
        readDelimitedMessage(type);
        // creating the message
        auto pMessage(MessageHelper::instance.newMessage(type.type()));
        // filling it
        readDelimitedMessage(*pMessage.get());
        // returning it
        holder.reset(new MessageCopy(pMessage, type.action()));
    }
public:
    ReadHelper(istream &stream_) :
        Helper<istream> (stream_, READHEADER) {
    }

    EIOState readHolder(SharedMessage &holder) throw () {
        try {
            readHolder_(holder);
            return EIOState::IO_SUCCESS_OK;
        } catch (EIOState &e) {
            return e;
        } catch (...) {
            cerr << header << "unknown exceptions caught in " << __FILE__ << "::" << __FUNCTION__ << endl;
            return IO_FATAL_ERROR;
        }
    }
};

class WriteHelper : public Helper<ostream> {
    void writeDelimitedMessage(const Message& msg) {
        msg.CheckInitialized();
        OstreamOutputStream streamWriter(&stream);
        CodedOutputStream codedOutputStream(&streamWriter);
        codedOutputStream.WriteVarint32(msg.ByteSize());
        // Perfectly safe to call SerializeWithCachedSizes as we called ByteSize on previous line
        // and msg is const.
        msg.SerializeWithCachedSizes(&codedOutputStream);
    }

    void writeHolder_(const SharedMessage &holder) {
        if (!holder)
            throw EIOState::IO_BAD_MESSAGE;
        checkInitialized(holder->message());
        writeDelimitedMessage(holder->messageType());
        writeDelimitedMessage(holder->message());
        check();
    }
public:
    WriteHelper(ostream &stream_) :
        Helper<ostream> (stream_, WRITEHEADER) {
    }

    EIOState writeHolder(const SharedMessage &holder) throw () {
        try {
            writeHolder_(holder);
            return EIOState::IO_SUCCESS_OK;
        } catch (EIOState &e) {
            return e;
        } catch (...) {
            cerr << header << "unknown exceptions caught in " << __FILE__ << "::" << __FUNCTION__ << endl;
            return IO_FATAL_ERROR;
        }
    }
};

/**
 * reads one message from the stream and populate the message holder accordingly
 * return true on success
 */
EIOState readHolder(SharedMessage &holder, istream& fromStream) throw () {
    return ReadHelper(fromStream).readHolder(holder);
}

/**
 * write the message holder to the stream
 * return true on success
 */
EIOState writeHolder(const SharedMessage &holder, ostream& toStream) throw () {
    return WriteHelper(toStream).writeHolder(holder);
}
