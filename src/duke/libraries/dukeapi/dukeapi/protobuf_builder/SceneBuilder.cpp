/*
 * SceneBuilder.cpp
 *
 *  Created on: 27 mars 2012
 *      Author: Guillaume Chatelet
 */

#include "SceneBuilder.h"

#include <player.pb.h>

#include <boost/bind.hpp>

#include <sstream>
#include <vector>
#include <set>
#include <map>

#include <functional>

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::serialize;
using namespace duke::playlist;
using namespace duke::protocol;

static vector<string> getTracks(const Playlist &playlist) {
    set<string> tracks;
    for (int i = 0; i < playlist.shot_size(); ++i)
        tracks.insert(playlist.shot(i).track());
    vector<string> sorted;
    copy(tracks.begin(), tracks.end(), back_inserter(sorted));
    sort(sorted.begin(), sorted.end());
    return sorted;
}

static inline Display getDefaultDisplay() {
    Display display;
    display.set_colorspace(Display::SRGB);
    return display;
}

Display update(const Display &local, const Display &with) {
    Display copy(local);
    if (!copy.has_colorspace() && with.has_colorspace())
        copy.set_colorspace(with.colorspace());
    return copy;
}

void normalize(Playlist &playlist) {
    const Display defaultDisplay = getDefaultDisplay();
    const Display globalDisplay = playlist.has_display() ? update(playlist.display(), defaultDisplay) : defaultDisplay;
    playlist.mutable_display()->CopyFrom(globalDisplay);
    for (int i = 0; i < playlist.shot_size(); ++i) {
        Shot &current = *playlist.mutable_shot(i);
        current.mutable_display()->CopyFrom(current.has_display() ? update(current.display(), globalDisplay) : globalDisplay);
    }
}

struct SceneBuilder {
    SceneBuilder(const Playlist &playlist) :
                    playlist(playlist) {
        const vector<string> tracks = getTracks(playlist);
        scene.mutable_track()->Reserve(tracks.size());
        for (vector<string>::const_iterator itr = tracks.begin(), end = tracks.end(); itr != end; ++itr) {
            const string &name(*itr);
            Track *pTrack = scene.add_track();
            pTrack->set_name(name);
            m_Track[name] = pTrack;
        }
    }
    ~SceneBuilder() {
        scene.PrintDebugString();
    }
    void handleShot(const Shot &shot) {
        Track &track = *m_Track[shot.track()];
        Clip &clip = *track.add_clip();
        ostringstream msg;
        msg << track.name() << '/' << track.clip_size();
        clip.set_name(msg.str());
        translate(shot, clip);
    }
private:
    void translate(const Shot &shot, Clip &clip) {
        Media &media = *clip.mutable_media();
        if (shot.has_mediastart() && shot.has_mediaend()) {
            media.set_type(Media::IMAGE_SEQUENCE);
            FrameRange &range = *media.mutable_source();
            range.set_first(shot.mediastart());
            range.set_last(shot.mediaend());
        } else {
            media.set_type(Media::SINGLE_IMAGE);
        }
        media.set_filename(shot.media());
        if (shot.has_reverse())
            media.set_reverse(shot.reverse());
        FrameRange &range = *clip.mutable_record();
        range.set_first(shot.trackstart());
        range.set_last(shot.trackend());
    }

    const Playlist &playlist;
    map<string, Track*> m_Track;
    Scene scene;
};

deque<google::protobuf::serialize::SharedHolder> getMessages(const Playlist &playlist) {
    const RepeatedPtrField<Shot> &shots = playlist.shot();
    SceneBuilder builder(playlist);
    for_each(shots.begin(), shots.end(), boost::bind(&SceneBuilder::handleShot, boost::ref(builder), _1));
    deque<SharedHolder> result;
    return result;
}

//static Scene createSceneFrom(const Playlist &playlist_) {
//    Playlist playlist(playlist_);
//    const map<string, size_t> tracks = getTracks(playlist);
//    Scene scene;
//    scene.mutable_track()->Reserve(tracks.size());
//    for (int i = 0; i < playlist.shot_size(); ++i) {
//        const Shot &shot = playlist.shot(i);
//        Track &track = *scene.mutable_track(tracks.find(shot.track())->second);
//    }
//    return scene;
//}
