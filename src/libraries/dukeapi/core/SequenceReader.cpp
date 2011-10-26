#include "SequenceReader.h"
#include "messageBuilder/Commons.h"

#include <dukeapi/sequence/Sequence.hpp>

#include <google/protobuf/text_format.h>
#include <player.pb.h>

#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>

#include <iostream>

using namespace std;
using namespace google::protobuf;
using namespace duke::protocol;

SequenceReader::SequenceReader(const string& directory, MessageQueue& queue, Playlist& _playlist) :
    m_Queue(queue) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    boost::filesystem::path p(directory);
    if (!::boost::filesystem::is_directory(p))
        throw std::runtime_error("Unable to find directory " + string(directory));

    // appending parameters
    addAutomaticParam(m_Queue, DISPLAY_DIM);
    addStaticFloatParam(m_Queue, DISPLAY_MODE, .0f);
    addStaticFloatParam(m_Queue, IMAGE_RATIO, .0f);

    // appending shapes
    addMesh(m_Queue, MS_Plane, "plane", -1, -1, 2, 2);

    size_t recin = 0;
    std::list<boost::shared_ptr<FileObject> > listing = fileObjectsInDir(p.string(), eMaskTypeSequence, eMaskOptionsNone );
    if(listing.empty())
        throw std::runtime_error("Unable to recognize any image sequence in " + string(directory));

    BOOST_FOREACH( const std::list<boost::shared_ptr<FileObject> >::value_type & s, listing )
    {
        Sequence *seq = dynamic_cast<Sequence*>(s.get());

        // adding clip
        Clip * pClip = addClipToPlaylist(//
                                         _playlist, //
                                         "clip0", //
                                         recin, //
                                         recin + seq->getDuration(), //
                                         seq->getFirstTime(), //
                                         directory, //
                                         seq->getStandardPattern());

        // computes next recin value
        recin += seq->getDuration();

        // appending parameters
        addAutomaticClipSourceParam(m_Queue, IMAGE_DIM, pClip->name());
        addStaticSamplerParam(m_Queue, "sampler", pClip->name());

        // appending default shaders to messages
        // ...vertex shader
        Shader vertexShader;
        buildVertexShader(vertexShader, "vs", fittableTransformVs, pClip->name());
        vertexShader.add_parametername(DISPLAY_DIM);
        vertexShader.add_parametername(IMAGE_DIM);
        vertexShader.add_parametername(DISPLAY_MODE);
        vertexShader.add_parametername(IMAGE_RATIO);
        push(m_Queue, vertexShader);

        // ...pixel shader
        Shader pixelShader;
        boost::filesystem::path pattern(seq->getStandardPattern());
        buildPixelShader(pixelShader, "ps", NULL, pClip->name());
        addShadingNode(pixelShader, "rgbatobgra", 1);
        if (pattern.extension() == ".dpx") {
            addShadingNode(pixelShader, "tenbitunpackfloat", 2);
        }
        push(m_Queue, pixelShader);

        // appending grading
        Grading * g = addGradingToClip(*pClip);
        addEffectToGrading(*g, pixelShader.name(), vertexShader.name(), "plane", true);
    }

    push(m_Queue, _playlist);
}
