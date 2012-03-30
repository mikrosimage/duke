#include "Configuration.h"
#include <dukeengine/CmdLineOptions.h>
#include <dukeengine/Version.h>
#include <dukeengine/host/io/ImageDecoderFactoryImpl.h>
#include <dukeapi/io/PlaybackReader.h>
#include <dukeapi/protobuf_builder/CmdLineParser.h>
#include <dukeapi/protobuf_builder/SceneBuilder.h>
#include <dukeapi/extension_set.hpp>
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
    ::duke::protocol::Renderer renderer;
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

        // renderer plugin path
        mSession->descriptor().setRendererPath(m_Vm[RENDERER].as<std::string>());

        // threading
        if (m_Vm.count(THREADS))
            mSession->descriptor().setThreadSize(m_Vm[THREADS].as<size_t>());

        // caching
        if (m_Vm.count(CACHESIZE))
            mSession->descriptor().setCacheSize((((uint64_t) m_Vm[CACHESIZE].as<size_t>()) * 1024) * 1024);

        // blanking / refreshrate
        renderer.set_presentinterval(m_Vm[BLANKING].as<unsigned>());
        renderer.set_refreshrate(m_Vm[REFRESHRATE].as<unsigned>());

        // checking renderer
        if (renderer.presentinterval() > 4)
            throw cmdline_exception(string(BLANKING) + " must be between 0 an 4");

        // checking command line
        const bool browseMode = m_Vm.count(BROWSE);
        const bool hasInputs = m_Vm.count(INPUTS);
        const vector<string> inputs = hasInputs ? m_Vm[INPUTS].as<vector<string> >() : vector<string>();

        // no special mode specified, using interactive mode
        MessageQueue & queue = mSession->descriptor().getInitTimeQueue();
        IOQueueInserter queueInserter(queue);

        // first, push the renderer msg
        queueInserter << renderer;

        // stopping rendering for now
        Engine stop;
        stop.set_action(Engine::RENDER_STOP);
        queueInserter << stop;

        const extension_set validExtensions = extension_set::create(mSession->factory().getAvailableExtensions());
        duke::playlist::Playlist playlist = browseMode ?  browseViewerComplete(validExtensions, inputs[0]) :  browsePlayer(validExtensions, inputs);

        if (m_Vm.count(FRAMERATE))
            playlist.set_framerate(m_Vm[FRAMERATE].as<unsigned int>());

        normalize(playlist);

        if (hasInputs) {
            if (browseMode && inputs.size() > 1)
                throw cmdline_exception("You are in browse mode, you must specify one and only one input.");

            const Scene::PlaybackMode mode = m_Vm.count(NOFRAMERATE) > 0 ? Scene::RENDER : m_Vm.count(NOSKIP) > 0 ? Scene::NO_SKIP : Scene::DROP_FRAME_TO_KEEP_REALTIME;
            vector<google::protobuf::serialize::SharedHolder> messages = getMessages(playlist, mode);
            queue.drainFrom(messages);

            if (playlist.has_startframe()) {
                duke::protocol::Transport cue;
                cue.set_type(Transport::CUE);
                cue.mutable_cue()->set_value(playlist.startframe());
                queueInserter << cue;
            }
        }

        // Push engine start
        Engine start;
        start.set_action(Engine::RENDER_START);
        queueInserter << start;
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
    cout << "\tOpen a sequence containing file       duke -s /path/to/img.1234.jpg" << endl;
    cout << "\tOpen a single file                    duke foo.jpg" << endl;
    cout << "\tOpen a playlist                       duke playlist.ppl" << endl;
    cout << "\tParse current folder for sequences    duke ." << endl;
    cout << "\tParse several folders for sequences   duke /path/to/dir1 /path/to/dir2" << endl << endl;
    cout << m_CmdlineOptionsGroup << endl;
}
