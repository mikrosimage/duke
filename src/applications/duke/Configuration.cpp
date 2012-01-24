#include "Configuration.h"
#include <dukeengine/Application.h>
#include <dukeengine/host/io/ImageDecoderFactoryImpl.h>
#include <dukeapi/core/messageBuilder/QuitBuilder.h>
#include <dukeapi/core/PlaybackReader.h>
#include <dukeapi/core/PlaylistReader.h>
#include <dukeapi/core/SequenceReader.h>
#include <dukeapi/core/FileRecorder.h>
#include <dukeapi/core/InteractiveMessageIO.h>
#include <dukeapi/io/SocketMessageIO.h>
#include <dukeapi/io/QueueMessageIO.h>
#include <dukeapi/serialize/ProtobufSocket.h>
#include <player.pb.h>
#include <protocol.pb.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "no information available - don't use in production"
#endif // BUILD_INFORMATION
// namespace
namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

// command line options
#define NOFRAMERATE         "no-framerate"
#define NOFRAMERATE_OPT     "no-framerate"
#define REFRESHRATE         "refreshrate"
#define REFRESHRATE_OPT     "refreshrate"
#define FULLSCREEN          "fullscreen"
#define FULLSCREEN_OPT      "fullscreen,f"
#define RESOLUTION          "resolution"
#define RESOLUTION_OPT      "resolution"
#define CACHESIZE           "cache-size"
#define CACHESIZE_OPT       "cache-size,c"
#define FRAMERATE           "framerate"
#define FRAMERATE_OPT       "framerate"
#define THREADS             "threads"
#define THREADS_OPT         "threads,t"
#define BLANKING            "blanking"
#define BLANKING_OPT        "blanking"
#define PLAYLIST            "playlist"
#define PLAYLIST_OPT        "playlist,p"
#define RENDERER            "renderer"
#define RENDERER_OPT        "renderer"
#define PLAYBACK            "playback"
#define PLAYBACK_OPT        "playback"
#define SEQUENCE            "sequence"
#define SEQUENCE_OPT        "sequence,s"
#define NOSKIP              "no-skip"
#define NOSKIP_OPT          "no-skip"
#define RECORD              "record"
#define RECORD_OPT          "record"
#define PORT                "port"
#define PORT_OPT            "port"

namespace { // empty namespace

void setDisplayOptions(boost::program_options::options_description& description, const ::duke::protocol::Renderer& Renderer) {
    ostringstream resolution;
    resolution << Renderer.width() << 'x' << Renderer.height();
    description.add_options() //
    (FULLSCREEN_OPT, "Sets the application to run fullscreen") //
    (BLANKING_OPT, po::value<unsigned int>()->default_value(1), "Blanking count before presentation, up to 4, 0 means immediate and results in tearing effect.") //
    (REFRESHRATE_OPT, po::value<unsigned int>()->default_value(Renderer.refreshrate()), "Forces the screen refresh rate (fullscreen mode)") //
    (RESOLUTION_OPT, po::value<string>()->default_value(resolution.str()), "Sets the dimensions of the display") //
     ;
}
struct SessionCreator {
    SessionCreator(QueueMessageIO& _io) :
            io(_io) {
    }
    google::protobuf::serialize::ISession* create(boost::asio::io_service& service) {
        return new SocketSession(service, io.inputQueue, io.outputQueue);
    }
private:
    QueueMessageIO& io;
};
const string HEADER = "[Configuration] ";

} // empty namespace

Configuration::Configuration(int argc, char** argv) :
        m_iReturnValue(EXIT_RELAUNCH),
        m_CmdLineOnly("command line only options"),
        m_Config("configuration options"),
        m_Display("display options"),
        m_Interactive("interactive mode options"),
        m_CmdlineOptionsGroup("Command line options"),
        m_ConfigFileOptions("Configuration file options"),
        m_HiddenOptions("hidden options")
{

    using namespace ::duke::protocol;

    // retrieving configuration file
    std::string configuration_filename;
    if (argc >= 1) {
        fs::path p(argv[0]);
        p.replace_extension(".ini");
        if (exists(p) && is_regular_file(p))
            configuration_filename = p.string();
    }

    // available on the command line
    m_CmdLineOnly.add_options() //
        ("help,h",  "Displays this help") //
        ("version", "Displays the version informations");

    // available in the configuration file and command line
    m_Config.add_options() //
        (RENDERER_OPT,      po::value<string>(),                            "Sets the renderer to be used") //
        (PLAYBACK_OPT,      po::value<string>(),                            "Play a recorded session back from file") //
        (RECORD_OPT,        po::value<string>(),                            "Record a session to file") //
        (PORT_OPT,          po::value<short>(),                             "Sets the port number to be used") //
        (CACHESIZE_OPT,     po::value<size_t>()->default_value(0),          "Cache size for preemptive read in MB. 0 means no caching.") //
        (THREADS_OPT,       po::value<size_t>()->default_value(1),          "Number of load/decode threads. Cache size must be >0.");
    // adding display settings
    ::duke::protocol::Renderer renderer;
    setDisplayOptions(m_Display, renderer);

    // adding interactive mode options
    m_Interactive.add_options() //
        (SEQUENCE_OPT,                                                      "Enable detection of sequences from filename") //
        (FRAMERATE_OPT,     po::value<unsigned int>()->default_value(25),   "Sets the playback framerate") //
        (NOFRAMERATE_OPT,                                                   "Reads the playlist as fast as possible. All images are displayed . Testing purpose only.") //
        (NOSKIP_OPT,                                                        "Try to keep the framerate but still ensures all images are displayed. Testing purpose only.");

    //adding hidden options
    m_HiddenOptions.add_options()
        ("inputs",          po::value< vector<string> >(),                  "input directories, files, sequences, playlists.");

    po::positional_options_description pod;
    pod.add("inputs", -1);

    // parsing the command line
    m_CmdlineOptionsGroup.add(m_CmdLineOnly).add(m_Config).add(m_Display).add(m_Interactive);
    boost::program_options::options_description parsingOptions;
    parsingOptions.add(m_CmdlineOptionsGroup).add(m_HiddenOptions);
    po::store(po::command_line_parser(argc, argv).options(parsingOptions).positional(pod).run(), m_Vm);

    // now parsing the configuration file - already stored variables will remain unchanged
    m_ConfigFileOptions.add(m_Config).add(m_Display).add(m_Interactive);
    ifstream configFile(configuration_filename.c_str(), ifstream::in);
    if (configFile.is_open()) {
        cout << HEADER + "Reading options from " << configuration_filename << endl;
        po::store(po::parse_config_file(configFile, m_ConfigFileOptions), m_Vm);
    }
    configFile.close();

    // notifying from incoming new options
    po::notify(m_Vm);

    if (m_Vm.count("help")) {
        displayHelp();
        return;
    }
    if (m_Vm.count("version")) {
        displayVersion();
        return;
    }
    if (m_Vm.count(RENDERER) == 0)
        throw runtime_error("No renderer specified. Aborting.");

    // loading plugins
    ImageDecoderFactoryImpl imageDecoderFactory;

    /**
     * Server mode
     */
    // if port is specified turning into a server
    if (m_Vm.count(PORT)) {
        using namespace boost::asio;
        using namespace boost::asio::ip;
        using google::protobuf::serialize::duke_server;
        while (m_iReturnValue == EXIT_RELAUNCH) {
            QueueMessageIO io;
            tcp::endpoint endpoint(tcp::v4(), m_Vm[PORT].as<short>());
            // -> c++0x version, cool but need a gcc version > 4.4
            //            auto sessionCreator = [&io](io_service &service) {return new SocketSession(service, io.inputQueue, io.outputQueue);};
            //            duke_server server(endpoint, sessionCreator);
            SessionCreator creator(io);
            duke_server server(endpoint, boost::bind(&SessionCreator::create, &creator, _1));
            boost::thread io_launcher(&duke_server::run, &server);
            decorateAndRun(io, imageDecoderFactory);
            io_launcher.join();
        }
        return;
    }

    /**
     * Playback mode
     */
    if (m_Vm.count(PLAYBACK)) {
        const string filename = m_Vm[PLAYBACK].as<string>();
        cout << HEADER + "Reading protocol buffer script: " << filename << endl;
        PlaybackReader decoder(filename.c_str());
        decorateAndRun(decoder, imageDecoderFactory);
        return;
    }

    /**
     * Interactive mode
     */
    renderer.set_presentinterval(m_Vm[BLANKING].as<unsigned>());
    renderer.set_fullscreen(m_Vm.count(FULLSCREEN) > 0);
    renderer.set_refreshrate(m_Vm[REFRESHRATE].as<unsigned>());
    if (m_Vm.count(RESOLUTION) != 0) {
        std::string res = m_Vm[RESOLUTION].as<string>();
        std::replace(res.begin(), res.end(), 'x', ' ');
        std::replace(res.begin(), res.end(), 'X', ' ');
        istringstream stream(res);
        int width = -1;
        int height = -1;
        stream >> width;
        stream >> height;
        if (stream.bad() || width == -1 || height == -1)
            throw runtime_error(std::string("bad resolution \"") + res + '\"');
        renderer.set_width(width);
        renderer.set_height(height);
    }

    // checking renderer
    if (renderer.presentinterval() > 4)
        throw runtime_error(string(BLANKING) + " must be between 0 an 4");

    Playlist playlist;
    const unsigned int framerate = m_Vm[FRAMERATE].as<unsigned int>();
    playlist.set_frameratenumerator((int) framerate);
    if (m_Vm.count(NOFRAMERATE) > 0)
        playlist.set_playbackmode(Playlist::RENDER);
    else if (m_Vm.count(NOSKIP) > 0)
        playlist.set_playbackmode(Playlist::NO_SKIP);
    else
        playlist.set_playbackmode(Playlist::DROP_FRAME_TO_KEEP_REALTIME);

    // no special mode specified, using interactive mode
    MessageQueue queue;
    push(queue, renderer);

    Engine stop;
    stop.set_action(Engine_Action_RENDER_STOP);
    push(queue, stop);

    if ( m_Vm.count("inputs") )
    {
        vector<string> inputs = m_Vm["inputs"].as< std::vector<std::string> >();

        for( vector<string>::iterator inputString = inputs.begin(); inputString!=inputs.end(); ++inputString )
        {
            //cout << *inputString << endl;
            int skip=0;
            int startRange = std::numeric_limits<int>::min();
            int endRange   = std::numeric_limits<int>::max();
            fs::path path( *inputString );
            if( path.extension() == ".ppl" || path.extension() == ".ppl2" )
            {
                //cout << HEADER + "Reading playlist: " << *inputString << endl;
                PlaylistReader( *inputString, queue, playlist );
            }
            else
            {
                cout << HEADER + "Reading : " << *inputString << endl;
                int inRange;
                int outRange;
                if( (++inputString < inputs.end()) && (inRange = std::atoi( (*inputString).c_str() )) )
                {
                    //cout << HEADER + "Range in : " << inRange << endl;
                    startRange = inRange;
                    skip++;

                    if( (++inputString < inputs.end()) && (outRange = std::atoi( (*inputString).c_str() ) ) )
                    {
                        //cout << HEADER + "Range out : " << outRange << endl;
                        endRange = outRange + 1;
                        skip++;
                    }
                    inputString--;
                }
                inputString--;
                if( startRange >= endRange )
                {
                    cout << "- ! -" << endl;
                    cout << "You should specify an valid range (" << startRange << " >= "<< endRange << "). See help below.\n" << endl;
                    displayHelp();
                    return;
                }
                SequenceReader( *inputString, imageDecoderFactory, queue, playlist, startRange, endRange, m_Vm.count("sequence") ? true : false );

                inputString += skip;
            }
        }
    }
    else {
        cout << "- ! -" << endl;
        cout << "You should specify an input (sequence directory or playlist file) in interactive mode. See help below.\n" << endl;
        displayHelp();
        return;
    }

    Engine start;
    start.set_action(Engine_Action_RENDER_START);
    push(queue, start);

    InteractiveMessageIO decoder(queue);
    decorateAndRun(decoder, imageDecoderFactory);
}

void Configuration::decorateAndRun(IMessageIO& io, ImageDecoderFactoryImpl &imageDecoderFactory) {
    if (m_Vm.count(RECORD) > 0) {
        const std::string recordFilename = m_Vm[RECORD].as<string>();
        FileRecorder recorder(recordFilename.c_str(), io);
        cout << HEADER + "recording session to " << recordFilename << endl;
        run(recorder, imageDecoderFactory);
    } else {
        run(io, imageDecoderFactory);
    }
}

void Configuration::run(IMessageIO& io, ImageDecoderFactoryImpl &imageDecoderFactory) {
    const std::string rendererFilename = m_Vm[RENDERER].as<string>();
    const uint64_t cacheSize = (((uint64_t) m_Vm[CACHESIZE].as<size_t>()) * 1024) * 1024;
    Application(rendererFilename.c_str(), imageDecoderFactory, io, m_iReturnValue, cacheSize, m_Vm[THREADS].as<size_t>());
}

void Configuration::displayVersion() {
    cout << "Duke Player" << endl;
    cout << BUILD_INFORMATION;
#if defined DEBUG
    cout << " - DEBUG" << endl;
#else
    cout << " - RELEASE" << endl;
#endif
}

void Configuration::displayHelp() {
    cout << "Usage: Duke image sequence viewer" << endl << endl;
    cout << "   Playlist:                                 duke playlist.ppl or duke playlist.ppl2" << endl;
    cout << "   One File:                                 duke foo.jpg" << endl;
    cout << "   This Directory:                           duke . or duke ./" << endl;
    cout << "   Other Directory:                          duke /path/to/dir" << endl;
    cout << "   Multi Directories:                        duke /path/to/dir1 /path/to/dir2" << endl;
    cout << "   Directories, files, sequences, playlist:  duke /path/to/dir /img/toto.###.jpg foo.jpg playlist.ppl2" << endl;

    cout << "   Fullscreen:                               duke --fullscreen input.ext" << endl << endl;

    cout << "Image Sequence Numbering" << endl << endl;

    cout << "   Frames with and without padding:          image.@.jpg" << endl;
    cout << "   Frames 1 to 100 padding 4:                image.####.jpg -or- image.@.jpg" << endl;
    cout << "   Frames 1 to 100 padding 5:                image.#####.jpg" << endl;
    cout << "   printf style padding 4:                   image.%04d.jpg" << endl;
    cout << "   printf style w/range:                     image.%04d.jpg 1 100" << endl;
    //cout << "   All Frames in Sequence:                   duke /path/to/dir /img/toto.###.jpg" << endl;
    cout << "   All Frames in Directory:                  /path/to/directory" << endl << endl;
    //cout << "   All Frames in current dir:                ." << endl << endl;

    cout << m_CmdlineOptionsGroup << endl;
}





