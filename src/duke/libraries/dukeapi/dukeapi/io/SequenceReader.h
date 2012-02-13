#ifndef SEQUENCEREADER_H_
#define SEQUENCEREADER_H_

#include <dukeapi/MessageQueue.h>

class ImageDecoderFactoryImpl;

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
    SequenceReader( int& clipIndex, int& recIn, const std::string&, const char** listOfExtensions, MessageQueue&, ::duke::protocol::Playlist&, const int startRange, const int endRange, bool detectionOfSequenceFromFilename = false );

private:
    MessageQueue& m_Queue;
    EInputType    inputType;
};

#endif /* SEQUENCEREADER_H_ */
