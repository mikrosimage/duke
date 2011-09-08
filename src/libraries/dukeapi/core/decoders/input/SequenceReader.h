/*
 * SequenceReader.h
 *
 *  Created on: 25 aout 2010
 *      Author: Nicolas Rondaud
 */

#ifndef SEQUENCEREADER_H_
#define SEQUENCEREADER_H_

#include <dukeapi/core/queue/MessageQueue.h>

namespace protocol {
namespace duke {
class Playlist;
} // namespace duke
} // namespace protocol

class SequenceReader{
public:
    SequenceReader(const std::string&, MessageQueue&, ::protocol::duke::Playlist&);

private:
    MessageQueue& m_Queue;
};

#endif /* SEQUENCEREADER_H_ */
