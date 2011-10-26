#ifndef SEQUENCEREADER_H_
#define SEQUENCEREADER_H_

#include <dukeapi/io/MessageQueue.h>

namespace duke {
namespace protocol {
class Playlist;
} // namespace protocol
} // namespace duke

class SequenceReader{
public:
    SequenceReader(const std::string&, MessageQueue&, ::duke::protocol::Playlist&);

private:
    MessageQueue& m_Queue;
};

#endif /* SEQUENCEREADER_H_ */
