#ifndef PLAYLISTNODE_H
#define PLAYLISTNODE_H

#include <dukexcore/dkxINode.h>
#include <dukexcore/dkxSessionDescriptor.h>
#include <dukeapi/core/PlaylistReader.h>
#include <dukeapi/core/SequenceReader.h>
#include <boost/filesystem.hpp>
#include "player.pb.h"

class PlaylistNode : public INode {

public:
    typedef boost::shared_ptr<PlaylistNode> ptr;
    PlaylistNode() :
        INode("fr.mikrosimage.dukex.playlist") {
    }

public:
    bool openFiles(const std::vector<std::string> & _list, const bool & asSequence = false) {
        try {
            SessionDescriptor & descriptor = session()->descriptor();
            ::duke::protocol::Playlist & playlist = descriptor.playlist();
            playlist.Clear();
            playlist.set_frameratenumerator(25);
            playlist.set_playbackmode(::duke::protocol::Playlist::NO_SKIP);
            MessageQueue queue;
            int clipIndex = 0; // use to generate clip name ( "clip" + clipName )
            int frameIndex = 0; // use to combine sequence/playlists
            for (std::vector<std::string>::const_iterator input = _list.begin(); input != _list.end(); ++input) {
                int skip = 0;
                int startRange = std::numeric_limits<int>::min();
                int endRange = std::numeric_limits<int>::max();
                boost::filesystem::path path(*input);
                if (path.extension() == ".ppl" || path.extension() == ".ppl2") {
                    PlaylistReader(clipIndex, frameIndex, *input, queue, playlist);
                } else {
                    int inRange;
                    int outRange;
                    if ((++input < _list.end()) && (inRange = std::atoi((*input).c_str()))) {
                        startRange = inRange;
                        skip++;
                        if ((++input < _list.end()) && (outRange = std::atoi((*input).c_str()))) {
                            endRange = outRange + 1;
                            skip++;
                        }
                        input--;
                    }
                    input--;
                    const char * ext[22] = { "pic", "pbm", "dpx", "tpic", "png", "pgm", "jpg", "pnm", "bmp", "exr", "sgi", "tiff", "tif", "dds", "tga", "jp2", "rgb", "j2k",
                                    "jpeg", "ico", "hdr", "ppm" };
                    SequenceReader(clipIndex, frameIndex, *input, ext, queue, playlist, startRange, endRange, asSequence);
                    input += skip;
                }
            }
            session()->sendMsg(queue);

        } catch (std::exception & e) {
            std::cerr << "[PlaylistNode] " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool setFramerate(float framerate) {
        try {
            SessionDescriptor & descriptor = session()->descriptor();
            ::duke::protocol::Playlist & p = descriptor.playlist();
            p.set_frameratenumerator((int) framerate);
            MessageQueue queue;
            push(queue, p);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool debug() {
        try {
            MessageQueue queue;
            ::duke::protocol::Debug d;
            d.add_line("%0");
            d.add_line("%1");
            d.add_line("%2");
            d.add_content(::duke::protocol::Debug_Content_FRAME);
            d.add_content(::duke::protocol::Debug_Content_FILENAMES);
            d.add_content(::duke::protocol::Debug_Content_FPS);
            push(queue, d, google::protobuf::serialize::MessageHolder::RETRIEVE);
            session()->sendMsg(queue);
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
};

#endif // PLAYLISTNODE_H
