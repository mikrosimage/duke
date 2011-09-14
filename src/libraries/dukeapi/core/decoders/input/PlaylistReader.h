#ifndef PLAYLISTREADER_H_
#define PLAYLISTREADER_H_

#include <dukeapi/protocol/playlist/playlist.pb.h>
#include <dukeapi/core/queue/MessageQueue.h>

namespace protocol {
namespace duke {
class Playlist;
} // namespace duke
} // namespace protocol

class PlaylistReader {
public:
    PlaylistReader(const std::string&, MessageQueue&, ::duke::protocol::Playlist&);

private:
    void parsePPL(std::ifstream & _file, ::duke::protocol::Playlist & _playlist);
    void parsePPL2(std::ifstream & _file, ::duke::protocol::Playlist & _playlist);
    MessageQueue& m_Queue;
};

#endif /* PLAYLISTREADER_H_ */
