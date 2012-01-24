#include "SequenceReader.h"

#include <dukeapi/core/messageBuilder/Commons.h>
#include <dukeengine/host/io/ImageDecoderFactoryImpl.h>

#include <Detector.hpp>

#include <google/protobuf/text_format.h>
#include <player.pb.h>

#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>

#include <iostream>

using namespace std;
using namespace google::protobuf;
using namespace duke::protocol;

static std::string INPUT = "Input";

SequenceReader::SequenceReader( const string& inputDirectory, ImageDecoderFactoryImpl &imageDecoderFactory, MessageQueue& queue, Playlist& _playlist, const int startRange, const int endRange, bool detectionOfSequenceFromFilename ) :
    m_Queue(queue) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    sequenceParser::Detector detector;


    boost::filesystem::path p( inputDirectory );
    string directory ( inputDirectory );

    if( ::boost::filesystem::exists( p )  )
    {
        if ( ::boost::filesystem::is_directory( p ) )
            inputType = eInputTypeDirectory;
        else
            inputType = eInputTypeFile;
    }
    else
    {
        inputType = eInputTypeSequence;
    }

    if ( inputType == eInputTypeSequence && !p.has_parent_path() )
    {
        directory.insert(0,"./");
    }
    else if ( inputType == eInputTypeSequence && !::boost::filesystem::is_directory( p.parent_path() ) )
    {
        throw std::runtime_error( "Unable to find " + string( directory ) );
    }

    // appending parameters
    addAutomaticParam(m_Queue, DISPLAY_DIM);
    addStaticFloatParam(m_Queue, DISPLAY_MODE, .0f);
    addStaticFloatParam(m_Queue, IMAGE_RATIO, .0f);

    // appending shapes
    addMesh(m_Queue, MS_Plane, "plane", -1, -1, 2, 2);

    size_t recin = 0;

    sequenceParser::EMaskOptions options = sequenceParser::eMaskOptionsNone;
    if( detectionOfSequenceFromFilename )
        options = sequenceParser::eMaskOptionsSequenceBasedOnFilename;

    std::list<boost::shared_ptr<sequenceParser::FileObject> > listing = detector.fileAndSequenceInDirectory ( directory, options );
    if( listing.empty() )
        throw std::runtime_error( "Unable to recognize any image sequence or image in " + string( directory ) );

    BOOST_FOREACH( const std::list<boost::shared_ptr<sequenceParser::FileObject> >::value_type & res, listing )
    {
        boost::filesystem::path pattern;
        Clip *                  pClip;

        if( res->getMaskType() == sequenceParser::eMaskTypeSequence )
        {
            // add the sequence to the playlist
            boost::shared_ptr<sequenceParser::Sequence> sequence = boost::static_pointer_cast<sequenceParser::Sequence> ( res );

            // check if one of plugins can read this format
            ::boost::filesystem::path sequenceName = sequence->getStandardPattern();
            bool delegateReadToHost, isUncompressed;
            if ( imageDecoderFactory.getImageDecoder(sequenceName.extension().string().c_str(), delegateReadToHost, isUncompressed) == NULL ) // decoder not found
                continue;

            int startPoint = std::max( startRange, (int)sequence->getFirstTime() );
            int stopPoint  = std::min( endRange, (int)( startPoint + sequence->getDuration() ) );

            std::cout <<  "[" << INPUT << "] " << sequence->getDirectory().string() << sequence->getStandardPattern() << " : " << startPoint << " to " << stopPoint << std::endl;

            // adding clip
            pClip = addClipToPlaylist(//
                    _playlist, //
                    "clip0", //
                    recin, //
                    recin + stopPoint - startPoint, //
                    startPoint, //
                    sequence->getDirectory().string(), //
                    sequence->getStandardPattern() );

            // computes next recin value
            recin += stopPoint - startPoint;

            pattern = sequence->getStandardPattern();
        }
        else
        {
            // add a file to the playlist
            boost::shared_ptr<sequenceParser::File> file = boost::static_pointer_cast<sequenceParser::File> ( res );

            // check if one of plugins can read this format
            ::boost::filesystem::path sequenceName = file->getFilename();
            bool delegateReadToHost, isUncompressed;
            if ( imageDecoderFactory.getImageDecoder(sequenceName.extension().string().c_str(), delegateReadToHost, isUncompressed) == NULL ) // decoder not found
                continue;

            std::cout << "[" << INPUT << "] " << file->getDirectory().string() << file->getFilename() << std::endl;

            // adding clip
            pClip = addClipToPlaylist(//
                    _playlist, //
                    "clip0", //
                    recin, //
                    recin+1, //
                    0, //
                    file->getDirectory().string(), //
                    file->getFilename() );

            // computes next recin value
            recin += 1;
            pattern = file->getAbsoluteFilename();
        }

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
        //addStaticFloatParam(m_Queue, IMAGE_RATIO, 16.f/9.f , pClip->name());
        push(m_Queue, vertexShader);

        // ...pixel shader
        Shader pixelShader;
        buildPixelShader(pixelShader, "ps", NULL, pClip->name());
        addShadingNode(pixelShader, "rgbatobgra", 1);
        if (pattern.extension() == ".dpx")
        {
            addShadingNode(pixelShader, "tenbitunpackfloat", 2);
        }
        push(m_Queue, pixelShader);

        // appending grading
        Grading * g = addGradingToClip(*pClip);
        addEffectToGrading(*g, pixelShader.name(), vertexShader.name(), "plane", true);
    }

    push(m_Queue, _playlist);
}
