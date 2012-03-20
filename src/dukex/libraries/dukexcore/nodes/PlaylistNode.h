#ifndef PLAYLISTNODE_H
#define PLAYLISTNODE_H

#include <dukexcore/dkxINode.h>
#include <dukexcore/dkxSessionDescriptor.h>
#include <dukeapi/protobuf_builder/CmdLinePlaylistBuilder.h>
#include <boost/filesystem.hpp>
#include "player.pb.h"

class PlaylistNode : public INode {

public:
    typedef boost::shared_ptr<PlaylistNode> ptr;
    PlaylistNode() :
        INode("fr.mikrosimage.dukex.playlist") {
    }

public:
    bool openFiles(const std::vector<std::string> inputs, const bool browseMode = false, const bool parseSequence = true) {
        try {
            SessionDescriptor & descriptor = session()->descriptor();

            // Save current playlist parameters
            ::duke::protocol::Playlist & playlist = descriptor.playlist();
            ::google::protobuf::uint32 framerate = playlist.frameratenumerator();
            ::duke::protocol::Playlist_PlaybackMode playbackmode = playlist.playbackmode();

            // Clear current playlist
            playlist.Clear();

            // Parse all inputs
            MessageQueue queue;
            IOQueueInserter inserter(queue);

            CmdLinePlaylistBuilder playlistBuilder(inserter, browseMode, parseSequence, session()->getAvailableExtensions());
            for_each(inputs.begin(), inputs.end(), playlistBuilder.appender());

            // Push engine stop
            ::duke::protocol::Engine stop;
            stop.set_action(::duke::protocol::Engine_Action_RENDER_STOP);
            inserter << stop;

            // Update and push the current playlist
            ::duke::protocol::Playlist newplaylist = playlistBuilder.getPlaylist();
            newplaylist.set_frameratenumerator(framerate);
            newplaylist.set_playbackmode(playbackmode);
            playlist = newplaylist;
            inserter << playlist;
            inserter << playlistBuilder.getCue();

            // Push engine start
            ::duke::protocol::Engine start;
            start.set_action(::duke::protocol::Engine_Action_RENDER_START);
            inserter << start;

            // send everything
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
//            MessageQueue queue;
//            ::duke::protocol::Debug d;
//            d.add_line("%0");
//            d.add_line("%1");
//            d.add_line("%2");
//            d.add_content(::duke::protocol::Debug_Content_FRAME);
//            d.add_content(::duke::protocol::Debug_Content_FILENAMES);
//            d.add_content(::duke::protocol::Debug_Content_FPS);
//            push(queue, d, google::protobuf::serialize::MessageHolder::RETRIEVE);
//            session()->sendMsg(queue);

            MessageQueue queue;
            ::duke::protocol::Info info;
            info.set_content(::duke::protocol::Info_Content_IMAGEINFO);
            push(queue, info, google::protobuf::serialize::MessageHolder::RETRIEVE);
            session()->sendMsg(queue);

        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
};

#endif // PLAYLISTNODE_H
