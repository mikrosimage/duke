#ifndef SEQUENCEREADER_H_
#define SEQUENCEREADER_H_

#include <dukeapi/io/MessageQueue.h>

namespace duke {
namespace protocol {
class Playlist;
} // namespace protocol
} // namespace duke


enum EInputType{
    eInputTypeDirectory = 0 ,
    eInputTypeSequence,
    eInputTypeFile
};

class SequenceReader{
public:
    SequenceReader( const std::string&, MessageQueue&, ::duke::protocol::Playlist&, bool detectionOfSequenceFromFilename = false );

private:
    MessageQueue& m_Queue;
    EInputType    inputType;
};

#endif /* SEQUENCEREADER_H_ */
