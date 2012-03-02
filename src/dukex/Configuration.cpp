#include "Configuration.h"
#include <dukeengine/host/io/ImageDecoderFactoryImpl.h>
#include <dukeapi/io/PlaybackReader.h>
#include <dukeapi/protobuf_builder/CmdLinePlaylistBuilder.h>
#include <dukeapi/io/InteractiveMessageIO.h>
#include <dukeapi/SocketMessageIO.h>
#include <dukeapi/QueueMessageIO.h>
#include <player.pb.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>

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
//#define PLAYBACK            "playback"
//#define PLAYBACK_OPT        "playback"
#define SEQUENCE            "sequence"
#define SEQUENCE_OPT        "sequence,s"
#define NOSKIP              "no-skip"
#define NOSKIP_OPT          "no-skip"
//#define RECORD              "record"
//#define RECORD_OPT          "record"
#define PORT                "port"
#define PORT_OPT            "port"
#define HELP                "help"
#define HELP_OPT            "help,h"
#define VERSION             "version"
#define VERSION_OPT         "version,v"

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
const string HEADER = "[Configuration] ";

} // empty namespace


Configuration::Configuration(Session::ptr s) :
    m_CmdLineOnly("command line only options"), //
                    m_Config("configuration options"), //
                    m_Display("display options"), //
                    m_Interactive("interactive mode options"), //
                    m_CmdlineOptionsGroup("Command line options"), //
                    m_ConfigFileOptions("Configuration file options"), //
                    m_HiddenOptions("hidden options"), //
                    mSession(s) {
}

bool Configuration::parse(int argc, char** argv) {
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
    (HELP_OPT, "Displays this help") //
    (VERSION_OPT, "Displays the version informations");

    // available in the configuration file and command line
    m_Config.add_options() //
        (RENDERER_OPT, po::value<string>(), "Sets the renderer to be used") //
    //    (PLAYBACK_OPT, po::value<string>(), "Play a recorded session back from file") //
    //    (RECORD_OPT, po::value<string>(), "Record a session to file") //
    (PORT_OPT, po::value<short>(), "Sets the port number to be used") //
        (CACHESIZE_OPT, po::value<size_t>()->default_value(0), "Cache size for preemptive read in MB. 0 means no caching.") //
        (THREADS_OPT, po::value<size_t>()->default_value(1), "Number of load/decode threads. Cache size must be >0.");

    // Adding display settings
    // Get Renderer from session descriptor
    ::duke::protocol::Renderer & renderer = mSession->descriptor().renderer();
    setDisplayOptions(m_Display, renderer);

    // adding interactive mode options
    m_Interactive.add_options() //
    (SEQUENCE_OPT, "Enable detection of sequences from filename") //
    (FRAMERATE_OPT, po::value<unsigned int>()->default_value(25), "Sets the playback framerate") //
    (NOFRAMERATE_OPT, "Reads the playlist as fast as possible. All images are displayed . Testing purpose only.") //
    (NOSKIP_OPT, "Try to keep the framerate but still ensures all images are displayed. Testing purpose only.");

    //adding hidden options
    m_HiddenOptions.add_options()("inputs", po::value<vector<string> >(), "input directories, files, sequences, playlists.");
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
    if (m_Vm.count(HELP)) {
        displayHelp();
        return false;
    }
    if (m_Vm.count(VERSION)) {
        displayVersion();
        return false;
    }
    if (m_Vm.count(RENDERER) == 0)
        throw runtime_error("No renderer specified. Aborting.");

    mSession->setRendererPath(m_Vm[RENDERER].as<std::string>());

    // threading
    if (m_Vm.count(THREADS))
        mSession->setThreadSize(m_Vm[THREADS].as<size_t>());

    // caching
    if (m_Vm.count(CACHESIZE))
        mSession->setCacheSize((((uint64_t) m_Vm[CACHESIZE].as<size_t>()) * 1024) * 1024);

    /**
     * Client mode
     */
    // if port is specified turning into a client
    if (m_Vm.count(PORT)) {
        mSession->setPort(m_Vm[PORT].as<short> ());
    }

    /**
     * Interactive mode
     */
    renderer.set_presentinterval(m_Vm[BLANKING].as<unsigned> ());
    renderer.set_fullscreen(m_Vm.count(FULLSCREEN) > 0);
    renderer.set_refreshrate(m_Vm[REFRESHRATE].as<unsigned> ());
    if (m_Vm.count(RESOLUTION) != 0) {
        std::string res = m_Vm[RESOLUTION].as<string> ();
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

    // Get Playlist from session descriptor
    Playlist & playlist = mSession->descriptor().playlist();

    const unsigned int framerate = m_Vm[FRAMERATE].as<unsigned int> ();
    playlist.set_frameratenumerator((int) framerate);
    if (m_Vm.count(NOFRAMERATE) > 0)
        playlist.set_playbackmode(Playlist::RENDER);
    else if (m_Vm.count(NOSKIP) > 0)
        playlist.set_playbackmode(Playlist::NO_SKIP);
    else
        playlist.set_playbackmode(Playlist::DROP_FRAME_TO_KEEP_REALTIME);

    // no special mode specified, using interactive mode
    MessageQueue & queue = mSession->getInitTimeMsgQueue();

    // Push engine stop
    ::duke::protocol::Engine stop;
    stop.set_action(::duke::protocol::Engine_Action_RENDER_STOP);
    push(queue, stop);

    if (m_Vm.count("inputs")) {
        const vector<string> inputs = m_Vm["inputs"].as<std::vector<std::string> > ();

        IOQueueInserter inserter(queue);
        CmdLinePlaylistBuilder playlistBuilder(inserter, m_Vm.count(SEQUENCE) > 0, mSession->getAvailableExtensions());
        for_each(inputs.begin(), inputs.end(), playlistBuilder.appender());

        // Push the new playlist
        Playlist newplaylist = playlistBuilder.getPlaylist();
        newplaylist.set_frameratenumerator(playlist.frameratenumerator());
        newplaylist.set_playbackmode(playlist.playbackmode());
        playlist = newplaylist;
        inserter << playlist;
        inserter << playlistBuilder.getCue();

    } else {
        push(queue, playlist);
    }

    // Push engine start
    ::duke::protocol::Engine start;
    start.set_action(::duke::protocol::Engine_Action_RENDER_START);
    push(queue, start);

    return true;
}

void Configuration::displayVersion() {
    cout << BUILD_INFORMATION;
#if defined DEBUG
    cout << " - DEBUG" << endl;
#else
    cout << " - RELEASE" << endl;
#endif
}

void Configuration::displayHelp() {
    cout << "Usage:" << endl;
    cout << "   Playlist:                                 dukex playlist.ppl or duke playlist.ppl2" << endl;
    cout << "   One File:                                 dukex foo.jpg" << endl;
    cout << "   This Directory:                           dukex . or duke ./" << endl;
    cout << "   Other Directory:                          dukex /path/to/dir" << endl;
    cout << "   Multi Directories:                        dukex /path/to/dir1 /path/to/dir2" << endl;
    cout << "   Directories, files, sequences, playlist:  dukex /path/to/dir /img/toto.###.jpg foo.jpg playlist.ppl2" << endl << endl;
    cout << "Image Sequence Numbering" << endl;
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

