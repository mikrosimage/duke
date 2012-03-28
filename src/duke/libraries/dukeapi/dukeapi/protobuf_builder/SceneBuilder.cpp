/*
 * SceneBuilder.cpp
 *
 *  Created on: 27 mars 2012
 *      Author: Guillaume Chatelet
 */

#include "SceneBuilder.h"
#include "details/MeshBuilder.h"
#include <dukeapi/messageBuilder/ShaderBuilder.h>
#include <dukeapi/messageBuilder/ParameterBuilder.h>

#include <player.pb.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <set>
#include <map>

#include <functional>

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::serialize;
using namespace duke::playlist;
using namespace duke::protocol;

static void setRange(FrameRange *pRange, uint32_t first, uint32_t last) {
    pRange->set_first(first);
    pRange->set_last(last);
}

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

static AutomaticParameter automaticTexDim(const string& name) {
    AutomaticParameter param;
    param.set_name(name);
    param.set_type(AutomaticParameter::FLOAT3_TEX_DIM);
    return param;
}

static void setClipSamplingSource(SamplingSource *pSource, const string &clipName){
    pSource->set_type(SamplingSource::CLIP);
    pSource->set_name(clipName);
}

static string tweakName(const string& name, const string &clipName){
    if(clipName.empty())
        return name;
    return clipName+"|"+name;
}

static AutomaticParameter automaticClipSource(const string& name, const string &clipName) {
    AutomaticParameter param = automaticTexDim(tweakName(name,clipName));
    setClipSamplingSource(param.mutable_samplingsource(), clipName);
    return param;
}

static void addSamplerState(StaticParameter &param, SamplerState_Type type, SamplerState_Value value) {
    SamplerState &state = *param.add_samplerstate();
    state.set_type(type);
    state.set_value(value);
}

static StaticParameter staticClipSampler(const string& name, const string &clipName) {
    StaticParameter param;
    param.set_name(tweakName(name,clipName));
    param.set_type(StaticParameter::SAMPLER);
    addSamplerState(param, SamplerState::MIN_FILTER, SamplerState::TEXF_POINT);
    addSamplerState(param, SamplerState::MAG_FILTER, SamplerState::TEXF_POINT);
    addSamplerState(param, SamplerState::WRAP_S, SamplerState::WRAP_BORDER);
    addSamplerState(param, SamplerState::WRAP_T, SamplerState::WRAP_BORDER);
    setClipSamplingSource(param.mutable_samplingsource(), clipName);
    return param;
}

struct SceneBuilder {
    SceneBuilder(const Playlist &playlist, float framerate, const duke::protocol::Scene::PlaybackMode mode) {
        const vector<string> tracks = getTracks(playlist);
        scene.set_playbackmode(mode);
        scene.set_frameratenumerator(framerate);
        scene.set_frameratedenominator(1);
        scene.mutable_track()->Reserve(tracks.size());
        for (vector<string>::const_iterator itr = tracks.begin(), end = tracks.end(); itr != end; ++itr) {
            const string &name(*itr);
            Track *pTrack = scene.add_track();
            pTrack->set_name(name);
            m_Track[name] = pTrack;
        }
    }

    void handleShot(const Shot &shot) {
        Track &track = *m_Track[shot.track()];
        Clip &clip = *track.add_clip();
        ostringstream msg;
        msg << track.name() << '/' << track.clip_size();
        clip.set_name(msg.str());
        setMedia(shot, clip);
        setGrading(shot, clip);
    }

    template<typename T>
    void packAndShare(const T& message) {
        result.push_back(google::protobuf::serialize::packAndShare(message));
//        message.PrintDebugString();
    }

    vector<SharedHolder> finish(){
        packAndShare(scene);
        return result;
    }
private:
    void setMedia(const Shot &shot, Clip &clip) {
        Media &media = *clip.mutable_media();
        if (shot.has_mediastart() && shot.has_mediaend()) {
            media.set_type(Media::IMAGE_SEQUENCE);
            setRange(media.mutable_source(), shot.mediastart(), shot.mediaend());
        } else {
            media.set_type(Media::SINGLE_IMAGE);
        }
        media.set_filename(shot.media());
        if (shot.has_reverse())
            media.set_reverse(shot.reverse());
        setRange(clip.mutable_record(), shot.trackstart(), shot.trackend());
    }

    void setGrading(const Shot &shot, Clip &clip) {
        Grading &grading = *clip.mutable_grade();
        RenderPass &pass = *grading.add_pass();
        pass.set_clean(true);
        pass.add_meshname(MeshBuilder::plane);
        Effect &effect = *pass.mutable_effect();
        const string psName = boost::iends_with(shot.media(), ".dpx") ? "ps_dpx" : "ps_normal";
        effect.set_pixelshadername(psName);
        effect.set_vertexshadername("vs");
        packAndShare(automaticClipSource(IMAGE_DIM, clip.name()));
        packAndShare(staticClipSampler("sampler", clip.name()));
    }

    map<string, Track*> m_Track;
    Scene scene;
public:
    vector<SharedHolder> result;
};

static StaticParameter staticFloat(const string& name, float value) {
    StaticParameter param;
    param.set_name(name);
    param.set_type(StaticParameter::FLOAT);
    param.add_floatvalue(value);
    return param;
}

vector<google::protobuf::serialize::SharedHolder> getMessages(const Playlist &playlist, const duke::protocol::Scene::PlaybackMode mode) {
    const RepeatedPtrField<Shot> &shots = playlist.shot();
    SceneBuilder builder(playlist, playlist.framerate(), mode);
    // mesh
    builder.packAndShare(MeshBuilder::buildPlane(MeshBuilder::plane));
    // appending unbound parameters
    builder.packAndShare(automaticTexDim(DISPLAY_DIM));
    builder.packAndShare(staticFloat(DISPLAY_MODE, 0));
    builder.packAndShare(staticFloat(IMAGE_RATIO, 0));
    builder.packAndShare(staticFloat(ZOOM, 1));
    builder.packAndShare(staticFloat(PANX, 0));
    builder.packAndShare(staticFloat(PANY, 0));

    Shader vertexShader;
    buildVertexShader(vertexShader, "vs", fittableTransformVs);
    vertexShader.add_parametername(DISPLAY_DIM);
    vertexShader.add_parametername(IMAGE_DIM);
    vertexShader.add_parametername(DISPLAY_MODE);
    vertexShader.add_parametername(IMAGE_RATIO);
    vertexShader.add_parametername(ZOOM);
    vertexShader.add_parametername(PANX);
    vertexShader.add_parametername(PANY);
    builder.packAndShare(vertexShader);

    Shader normalPS;
    buildPixelShader(normalPS, "ps_normal", NULL);
    addShadingNode(normalPS, "rgbatobgra", 1);
    builder.packAndShare(normalPS);

    Shader dpxPS;
    buildPixelShader(dpxPS, "ps_dpx", NULL);
    addShadingNode(dpxPS, "rgbatobgra", 1);
    addShadingNode(dpxPS, "tenbitunpackfloat", 2);
    builder.packAndShare(dpxPS);

    for_each(shots.begin(), shots.end(), boost::bind(&SceneBuilder::handleShot, boost::ref(builder), _1));

    return builder.finish();
}
