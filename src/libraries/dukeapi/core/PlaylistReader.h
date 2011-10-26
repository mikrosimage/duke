#ifndef PLAYLISTREADER_H_
#define PLAYLISTREADER_H_

#include <dukeapi/io/MessageQueue.h>
#include <string>

namespace duke {
namespace protocol {
class Playlist;
} // namespace protocol
} // namespace duke

class PlaylistReader {
public:
    PlaylistReader(const std::string&, MessageQueue&, ::duke::protocol::Playlist&);

private:
    void parsePPL(std::ifstream & _file, ::duke::protocol::Playlist & _playlist);
    void parsePPL2(std::ifstream & _file, ::duke::protocol::Playlist & _playlist);
    MessageQueue& m_Queue;
};

#endif /* PLAYLISTREADER_H_ */
