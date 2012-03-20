#include "Configuration.h"
#include <dukeengine/Version.h>
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

// namespace
namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

// command line options
static const char* const NOFRAMERATE = "no-framerate";
static const char* const NOFRAMERATE_OPT = NOFRAMERATE;
static const char* const REFRESHRATE = "refreshrate";
static const char* const REFRESHRATE_OPT = REFRESHRATE;
static const char* const CACHESIZE = "cache-size";
static const char* const CACHESIZE_OPT = "cache-size,c";
static const char* const FRAMERATE = "framerate";
static const char* const FRAMERATE_OPT = FRAMERATE;
static const char* const THREADS = "threads";
static const char* const THREADS_OPT = "threads,t";
static const char* const BLANKING = "blanking";
static const char* const BLANKING_OPT = BLANKING;
static const char* const RENDERER = "renderer";
static const char* const RENDERER_OPT = RENDERER;
static const char* const VERSION = "version";
static const char* const VERSION_OPT = VERSION;
static const char* const BROWSE = "browse";
static const char* const BROWSE_OPT = "browse,b";
static const char* const NOSKIP = "no-skip";
static const char* const NOSKIP_OPT = NOSKIP;
static const char* const INPUTS = "inputs";
static const char* const HELP = "help";
static const char* const HELP_OPT = "help,h";

struct cmdline_exception : public runtime_error {
    cmdline_exception(const string &msg) :
                    runtime_error(msg) {
    }
};

namespace { // empty namespace

void setDisplayOptions(boost::program_options::options_description& description, const ::duke::protocol::Renderer& Renderer) {
    ostringstream resolution;
    resolution << Renderer.width() << 'x' << Renderer.height();
    description.add_options() //
    //    (FULLSCREEN_OPT, "Sets the application to run fullscreen") //
    (BLANKING_OPT, po::value<unsigned int>()->default_value(1), "Blanking count before presentation, up to 4, 0 means immediate and results in tearing effect.") //
    (REFRESHRATE_OPT, po::value<unsigned int>()->default_value(Renderer.refreshrate()), "Forces the screen refresh rate (fullscreen mode)") //
     //    (RESOLUTION_OPT, po::value<string>()->default_value(resolution.str()), "Sets the dimensions of the display") //
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
    (CACHESIZE_OPT, po::value<size_t>()->default_value(0), "Cache size for preemptive read in MB. 0 means no caching.") //
    (THREADS_OPT, po::value<size_t>()->default_value(1), "Number of load/decode threads. Cache size must be >0.");

    // Adding display settings
    // Get Renderer from session descriptor
    ::duke::protocol::Renderer & renderer = mSession->descriptor().renderer();
    setDisplayOptions(m_Display, renderer);

    // adding interactive mode options
    m_Interactive.add_options() //
    (BROWSE_OPT, "Browse mode, act as an image browser") //
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

    try {

        if (m_Vm.count(HELP)) {
            displayHelp();
            return false;
        }
        if (m_Vm.count(VERSION)) {
            displayVersion();
            return false;
        }
        if (m_Vm.count(RENDERER) == 0)
            throw cmdline_exception("No renderer specified. Aborting.");

        mSession->setRendererPath(m_Vm[RENDERER].as<std::string>());

        // threading
        if (m_Vm.count(THREADS))
            mSession->setThreadSize(m_Vm[THREADS].as<size_t>());

        // caching
        if (m_Vm.count(CACHESIZE))
            mSession->setCacheSize((((uint64_t) m_Vm[CACHESIZE].as<size_t>()) * 1024) * 1024);

        /**
         * Interactive mode
         */
        renderer.set_presentinterval(m_Vm[BLANKING].as<unsigned>());
        renderer.set_refreshrate(m_Vm[REFRESHRATE].as<unsigned>());

        // checking renderer
        if (renderer.presentinterval() > 4)
            throw cmdline_exception(string(BLANKING) + " must be between 0 an 4");

        // no special mode specified, using interactive mode
        MessageQueue & queue = mSession->getInitTimeMsgQueue();

        // Push engine stop
        ::duke::protocol::Engine stop;
        stop.set_action(::duke::protocol::Engine_Action_RENDER_STOP);
        push(queue, stop);

        const bool browseMode = m_Vm.count(BROWSE);
        const bool hasInputs = m_Vm.count(INPUTS);
        const vector<string> inputs = hasInputs ? m_Vm[INPUTS].as<vector<string> >() : vector<string>();

        if (hasInputs) {
            if (browseMode && inputs.size() > 1)
                throw cmdline_exception("You are in browse mode, you must specify one and only one input.");

            IOQueueInserter queueInserter(queue);

            CmdLinePlaylistBuilder playlistBuilder(queueInserter, browseMode, mSession->getAvailableExtensions());

            for_each(inputs.begin(), inputs.end(), playlistBuilder.appender());

            // Push the new playlist
            Playlist playlist = playlistBuilder.getPlaylist();

            const unsigned int framerate = m_Vm[FRAMERATE].as<unsigned int>();
            playlist.set_frameratenumerator((int) framerate);
            if (m_Vm.count(NOFRAMERATE) > 0)
                playlist.set_playbackmode(Playlist::RENDER);
            else if (m_Vm.count(NOSKIP) > 0)
                playlist.set_playbackmode(Playlist::NO_SKIP);
            else
                playlist.set_playbackmode(Playlist::DROP_FRAME_TO_KEEP_REALTIME);

            queueInserter << playlist;
            queueInserter << playlistBuilder.getCue();

            mSession->descriptor().playlist() = playlist; // updating playlist
        } else {
            push(queue, mSession->descriptor().playlist());
        }

        // Push engine start
        ::duke::protocol::Engine start;
        start.set_action(::duke::protocol::Engine_Action_RENDER_START);
        push(queue, start);
    } catch (cmdline_exception &e) {
        cout << "invalid command line : " << e.what() << endl << endl;
        displayHelp();
        return false;
    }
    return true;
}

void Configuration::displayVersion() {
    cout << getVersion("DukeX") << endl;
}

void Configuration::displayHelp() {
    cout << "Usage: " << endl;
    cout << "\tOpen a file in browse mode            duke -b /path/to/img.1234.jpg" << endl;
    cout << "\tOpen a folder in browse mode          duke -b /path/to" << endl;
    cout << "\tOpen a single file                    duke foo.jpg" << endl;
    cout << "\tOpen a playlist                       duke playlist.ppl" << endl;
    cout << "\tParse current folder for sequences    duke ." << endl;
    cout << "\tParse several folders for sequences   duke /path/to/dir1 /path/to/dir2" << endl << endl;
    cout << m_CmdlineOptionsGroup << endl;
}
