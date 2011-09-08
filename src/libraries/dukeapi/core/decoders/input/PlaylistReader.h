/*
 * PlaylistReader.h
 *
 *  Created on: 25 aout 2010
 *      Author: Nicolas Rondaud
 */

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
    PlaylistReader(const std::string&, MessageQueue&, ::protocol::duke::Playlist&);

private:
    void parsePPL(std::ifstream & _file, ::protocol::duke::Playlist & _playlist);
    void parsePPL2(std::ifstream & _file, ::protocol::duke::Playlist & _playlist);
    MessageQueue& m_Queue;
};

#endif /* PLAYLISTREADER_H_ */
