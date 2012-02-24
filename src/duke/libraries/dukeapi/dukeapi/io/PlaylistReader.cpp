#include "PlaylistReader.h"

#include <dukeapi/messageBuilder/Commons.h>
#include <player.pb.h>
#include <playlist.pb.h>

#include <google/protobuf/text_format.h>

#include <boost/filesystem/path.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

using namespace ::duke::protocol;
using namespace ::std;

namespace { // empty namespace

static bool isEmptyOrComment(const string& line) {
    if (line.empty())
        return true;
    for (string::const_iterator itr = line.begin(); itr != line.end(); ++itr) {
        locale loc;
        if (isspace(*itr, loc))
            continue;
        else if (isdigit(*itr, loc))
            return false;
        else if ('#' == *itr)
            return true;
        else
            return true;
    }
    return true;
}

static float getImageRatio(::duke::playlist::Display_ImageRatio _ratio) {
    switch (_ratio) {
        case ::duke::playlist::Display_ImageRatio__1_1:
            return 1.0f;
        case ::duke::playlist::Display_ImageRatio__4_3:
            return 4.f / 3.f;
        case ::duke::playlist::Display_ImageRatio__16_9:
            return 16.f / 9.f;
        default:
            assert(!"Aspect ratio not handled.");
            cerr << "Aspect ratio not handled." << endl;
            return .0f;
    }
    return .0f;
}

} // empty namespace

// TODO : use sequence/PlaylistBuilder instead
static inline Clip & addClipToPlaylist(dk::Playlist & _p //
                                   , string _name = "defaultClipName" //
                                   , size_t _rec_first = 0 //
                                   , size_t _rec_last = 1 //
                                   , size_t _src_first = 0 //
                                   , string _filename = "") {
    Track &track = _p.track_size()==0 ? *_p.add_track() : *_p.mutable_track(0);
    Clip &clip = *track.add_clip();
    clip.set_name(_name);
    FrameRange &record = *clip.mutable_record();
    record.set_first(_rec_first);
    record.set_last(_rec_last);
    Media &media = *clip.mutable_media();
    media.set_type(Media_Type_IMAGE_SEQUENCE);
    media.set_filename(_filename);
    FrameRange &source = *media.mutable_source();
    source.set_first(_src_first);
    source.set_last(_src_first + _rec_last - _rec_first);
    return clip;
}

PlaylistReader::PlaylistReader( int& clipIndex, int& recIn, const string& filename, MessageQueue& queue, Playlist& playlist ) :
    m_Queue(queue) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    ifstream playlistFile(filename.c_str(), ifstream::in);
    if (!playlistFile.is_open())
        throw runtime_error("Unable to open playlist file " + filename);

    // appending parameters
    addAutomaticParam(m_Queue, DISPLAY_DIM);
    addStaticFloatParam(m_Queue, DISPLAY_MODE, .0f);
    addStaticFloatParam(m_Queue, IMAGE_RATIO, .0f);
    addStaticFloatParam(m_Queue, ZOOM, 1.0f);
    addStaticFloatParam(m_Queue, PANX, 0.0f);
    addStaticFloatParam(m_Queue, PANY, 0.0f);

    // appending shapes
    addMesh(m_Queue, MS_Plane, "plane", -1, -1, 2, 2);

    // initializing playlist
    try {
        ::boost::filesystem::path playlistPath(filename);
        if (playlistPath.extension() == ".ppl")
            parsePPL(clipIndex, recIn, playlistFile, playlist);
        else if (playlistPath.extension() == ".ppl2")
            parsePPL2(clipIndex, recIn, playlistFile, playlist);
        else
            throw runtime_error(string("invalid playlist format \"") + playlistPath.extension().string() + "\"\n");
    } catch (exception & e) {
        cerr << "Error in playlist file " << filename << endl;
        cerr << e.what() << endl;
    }
    push(m_Queue, playlist);
}


// private
void PlaylistReader::parsePPL(int& clipIndex, int& recIn, ifstream & _file, Playlist & _playlist) {
        string line;
        int initialRecIn = recIn;
        while (getline(_file, line)) {
        if (isEmptyOrComment(line))
            continue;
        istringstream lineStream(line, ios_base::in);
        int start, in, out;
        string pattern;
        lineStream >> start;
        lineStream >> pattern;
        lineStream >> in;
        lineStream >> out;

        // is line valid ?
        if (lineStream.fail())
            throw runtime_error(string("invalid line ->\'") + line + "\'");

        // removing leading and trailing '"'
        const char firstChar = *pattern.begin();
        const char lastChar = *pattern.rbegin();
        if (firstChar == lastChar && firstChar == '"')
            pattern = pattern.substr(1, pattern.size() - 2);

        // naming
        stringstream ssClip;
        ssClip << "clip" << clipIndex++;

        // adding clip
        ::boost::filesystem::path path(pattern);
        //std::cout <<  "[" << INPUT << "-" << ssClip.str() << "] " << path.string() << " : " << start << " to " << start + out - in + 1 << std::endl;
        Clip &clip = addClipToPlaylist(//
                            _playlist, //
                            ssClip.str(), //
                            initialRecIn + recIn - start, //
                            initialRecIn + recIn - start + out - in, //
                            in, //
                            path.string());

//        recIn += out - in;

        // appending parameters
        addAutomaticClipSourceParam(m_Queue, IMAGE_DIM, clip.name());
        addStaticSamplerParam(m_Queue, "sampler", clip.name());

        // appending default shaders to messages
        // ...vertex shader
        Shader vertexShader;
        buildVertexShader(vertexShader, "vs", fittableTransformVs, clip.name());
        vertexShader.add_parametername(DISPLAY_DIM);
        vertexShader.add_parametername(IMAGE_DIM);
        vertexShader.add_parametername(DISPLAY_MODE);
        vertexShader.add_parametername(IMAGE_RATIO);
        vertexShader.add_parametername(ZOOM);
        vertexShader.add_parametername(PANX);
        vertexShader.add_parametername(PANY);
        push(m_Queue, vertexShader);

        // ...pixel shader
        Shader pixelShader;
        buildPixelShader(pixelShader, "ps", NULL, clip.name());

                addShadingNode(pixelShader, "rgbatobgra", 1);
                if (path.extension() == ".dpx") {
                        addShadingNode(pixelShader, "tenbitunpackfloat", 2);
                }

        push(m_Queue, pixelShader);

        // appending grading
        Grading * g = addGradingToClip(clip);
        addEffectToGrading(*g, pixelShader.name(), vertexShader.name(), "plane", true);
    }
}

// private
void PlaylistReader::parsePPL2(int& clipIndex, int& recIn, ifstream & _file, Playlist & _playlist) {
    int initialRecIn = recIn;
    stringstream ss;
    ss << _file.rdbuf();
    string data = ss.str();

    ::duke::playlist::Playlist p;
    ::google::protobuf::TextFormat::ParseFromString(data, &p);

    for (int i = 0; i < p.shot_size(); ++i) {
        stringstream ssClip;
        ssClip << "clip" << clipIndex++;

        // adding sound
        if (p.has_audiosource()) {
            const ::duke::playlist::AudioSource & s = p.audiosource();
            if (s.has_name()) {
                AudioSource * source = _playlist.mutable_audiosource();
                source->set_name(s.name());
            }
        }

        // adding clip
        ::boost::filesystem::path path(p.shot(i).path());
        //std::cout <<  "[" << INPUT << "-" << ssClip.str() << "] " << path.string() << " : " << p.shot(i).start() << " to " << p.shot(i).start() + p.shot(i).out() - p.shot(i).in() + 1 << std::endl;
        Clip &clip = addClipToPlaylist(_playlist, //
                                         ssClip.str(), //
                                         initialRecIn + p.shot(i).start(), //
                                         initialRecIn + p.shot(i).start() + p.shot(i).out() - p.shot(i).in(), //
                                         p.shot(i).in(), //
                                         path.string());

//        recIn += p.shot(i).out() - p.shot(i).in();

        // appending parameters
        addAutomaticClipSourceParam(m_Queue, IMAGE_DIM, clip.name());
        addStaticSamplerParam(m_Queue, "sampler", clip.name());

        // appending default shaders
        // ...vertex shader
        Shader vertexShader;
        buildVertexShader(vertexShader, "vs", fittableTransformVs, clip.name());
        vertexShader.add_parametername(DISPLAY_DIM);
        vertexShader.add_parametername(IMAGE_DIM);
        vertexShader.add_parametername(DISPLAY_MODE);
        vertexShader.add_parametername(IMAGE_RATIO);
        vertexShader.add_parametername(ZOOM);
        vertexShader.add_parametername(PANX);
        vertexShader.add_parametername(PANY);
        push(m_Queue, vertexShader);

        // ...pixel shader
        Shader pixelShader;
        buildPixelShader(pixelShader, "ps", NULL, clip.name());
        addShadingNode(pixelShader, "rgbatobgra", 1);
        size_t nextNode = 2;

        if (path.extension() == ".dpx") {
            addShadingNode(pixelShader, "tenbitunpackfloat", 2);
            nextNode = 3;
        }
        if (p.shot(i).has_display()) {
            if (p.shot(i).display().has_imageratio())
                addStaticFloatParam(m_Queue, IMAGE_RATIO, getImageRatio(p.shot(i).display().imageratio()), clip.name());

            if (p.shot(i).display().has_colorspace()) {
                switch (p.shot(i).display().colorspace()) {
                    case ::duke::playlist::Display_ColorSpace_LOGTOLIN:
                        addShadingNode(pixelShader, "logtolin", nextNode);
                        break;
                    case ::duke::playlist::Display_ColorSpace_SRGBTOLIN:
                        addShadingNode(pixelShader, "srgbtolin", nextNode);
                        break;
                    case ::duke::playlist::Display_ColorSpace_LINTOLOG:
                        addShadingNode(pixelShader, "lintolog", nextNode);
                        break;
                    case ::duke::playlist::Display_ColorSpace_LINTOSRGB:
                        addShadingNode(pixelShader, "lintosrgb", nextNode);
                        break;
                    case ::duke::playlist::Display_ColorSpace_FULLHEADTOSMPTE:
                        addShadingNode(pixelShader, "fullheadtosmpte", nextNode);
                        break;
                    case ::duke::playlist::Display_ColorSpace_SMPTETOFULLHEAD:
                        addShadingNode(pixelShader, "smptetofullhead", nextNode);
                        break;
                    default:
                        assert(!"Colorspace format not handled.");
                        std::cerr << "Colorspace format not handled." << std::endl;
                        break;
                }
            }
        }
        push(m_Queue, pixelShader);

        // appending grading
        Grading * g = addGradingToClip(clip);
        addEffectToGrading(*g, pixelShader.name(), vertexShader.name(), "plane", true);
    }
}
