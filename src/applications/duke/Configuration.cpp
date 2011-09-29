#include "Configuration.h"
#include <dukeengine/Application.h>
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
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "no information available - don't use in production"
#endif // BUILD_INFORMATION
namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

const string HEADER = "[Configuration] ";
const char* NO_FRAMERATE = "no-framerate";
const char* REFRESHRATE = "refreshrate";
const char* FULLSCREEN = "fullscreen";
const char* RESOLUTION = "resolution";
const char* CACHE_SIZE = "cache-size";
const char* FRAMERATE = "framerate";
const char* BLANKING = "blanking";
const char* PLAYLIST = "playlist";
const char* RENDERER = "renderer";
const char* PLAYBACK = "playback";
const char* SEQUENCE = "sequence";
const char* NO_SKIP = "no-skip";
const char* RECORD = "record";
const char* PORT = "port";

void setDisplayOptions(boost::program_options::options_description& description, const ::duke::protocol::Renderer& Renderer) {
    ostringstream resolution;
    resolution << Renderer.width() << 'x' << Renderer.height();

    description.add_options() //
    (FULLSCREEN, "Sets the application to run fullscreen") //
    (BLANKING, po::value<unsigned int>()->default_value(1), "Blanking count before presentation, up to 4, 0 means immediate and results in tearing effect.") //
    (REFRESHRATE, po::value<unsigned int>()->default_value(Renderer.refreshrate()), "Forces the screen refresh rate (fullscreen mode)") //
    (RESOLUTION, po::value<string>()->default_value(resolution.str()), "Sets the dimensions of the display") //
    ;
}

Configuration::Configuration(int argc, char** argv) :
    m_iReturnValue(0), m_CmdLineOnly("command line only options"), m_Config("configuration options"), m_Display("display options"), m_Interactive("interactive mode options"),
            m_CmdlineOptionsGroup("Command line options"), m_ConfigFileOptions("Configuration file options") {

    using namespace ::duke::protocol;

    std::string configuration_filename;
    if (argc >= 1) {
        fs::path p(argv[0]);
        p.replace_extension(".ini");
        if (exists(p) && is_regular_file(p))
            configuration_filename = p.string();
    }

    // available on the command line
    m_CmdLineOnly.add_options() //
    ("help,h", "Displays this help") //
    ("version", "Displays the version informations");
    // available in the configuration file and command line
    m_Config.add_options() //
    (RENDERER, po::value<string>(), "Sets the renderer to be used") //
    (PLAYBACK, po::value<string>(), "Play a recorded session back from file") //
    (RECORD, po::value<string>(), "Record a session to file") //
    (PORT, po::value<short>(), "Sets the port number to be used") //
    (CACHE_SIZE, po::value<size_t>()->default_value(0), "Cache size for preemptive read in MB. 0 means no caching.");
    // adding display settings
    ::duke::protocol::Renderer renderer;
    setDisplayOptions(m_Display, renderer);
    // adding interactive mode options
    m_Interactive.add_options() //
    (PLAYLIST, po::value<string>(), "Adds a playlist file as input (.ppl, .ppl2)") //
    (SEQUENCE, po::value<string>(), "Adds a sequence directory as input") //
    (FRAMERATE, po::value<unsigned int>()->default_value(25), "Sets the playback framerate") //
    (NO_FRAMERATE, "Reads the playlist as fast as possible. All images are displayed . Testing purpose only.") //
    (NO_SKIP, "Try to keep the framerate but still ensures all images are displayed. Testing purpose only.");

    // parsing the command line
    m_CmdlineOptionsGroup.add(m_CmdLineOnly).add(m_Config).add(m_Display).add(m_Interactive);
    po::store(po::parse_command_line(argc, argv, m_CmdlineOptionsGroup), m_Vm);

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

    // if port is specified turning into a server
    if (m_Vm.count(PORT)) {
        using namespace boost::asio;
        using namespace boost::asio::ip;
        using google::protobuf::serialize::duke_server;

        while (m_iReturnValue == 0) {
            QueueMessageIO io;

            auto sessionCreator = [&io](io_service &service) {return new SocketSession(service, io.inputQueue, io.outputQueue);};

            tcp::endpoint endpoint(tcp::v4(), m_Vm[PORT].as<unsigned short> ());
            duke_server server(endpoint, sessionCreator);

            boost::thread io_launcher(&duke_server::run, &server);

            decorateAndRun(io);

            io_launcher.join();
        }
        return;
    }

    if (m_Vm.count(PLAYBACK)) {
        const string filename = m_Vm[PLAYBACK].as<string> ();
        cout << HEADER + "Reading protocol buffer script: " << filename << endl;
        PlaybackReader decoder(filename.c_str());
        decorateAndRun(decoder);
        return;
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

    Playlist playlist;

    const unsigned int framerate = m_Vm[FRAMERATE].as<unsigned int> ();
    playlist.set_frameratenumerator((int) framerate);
    if (m_Vm.count(NO_FRAMERATE) > 0)
        playlist.set_playbackmode(Playlist::RENDER);
    else if (m_Vm.count(NO_SKIP) > 0)
        playlist.set_playbackmode(Playlist::NO_SKIP);
    else
        playlist.set_playbackmode(Playlist::DROP_FRAME_TO_KEEP_REALTIME);

    // no special mode specified, using interactive mode
    MessageQueue queue;
    push(queue, renderer);

    Engine stop;
    stop.set_action(Engine_Action_RENDER_STOP);
    push(queue, stop);

    if (m_Vm.count(SEQUENCE)) {
        const string directory = m_Vm[SEQUENCE].as<string> ();
        cout << HEADER + "Reading directory: " << directory << endl;
        SequenceReader(directory, queue, playlist);
    } else if (m_Vm.count(PLAYLIST)) {
        const string file = m_Vm[PLAYLIST].as<string> ();
        cout << HEADER + "Reading playlist: " << file << endl;
        PlaylistReader(file, queue, playlist);
    } else {
        throw runtime_error("You should specify an input (sequence directory or playlist file) in interactive mode. Aborting.");
    }

    Engine start;
    start.set_action(Engine_Action_RENDER_START);
    push(queue, start);

    InteractiveMessageIO decoder(queue);
    decorateAndRun(decoder);
}

void Configuration::decorateAndRun(IMessageIO& io) {
    if (m_Vm.count(RECORD) > 0) {
        const std::string recordFilename = m_Vm[RECORD].as<string> ();
        FileRecorder recorder(recordFilename.c_str(), io);
        cout << HEADER + "recording session to " << recordFilename << endl;
        run(recorder);
    } else {
        run(io);
    }
}
void Configuration::run(IMessageIO& io) {
    const std::string rendererFilename = m_Vm[RENDERER].as<string> ();
    const size_t cacheSize = m_Vm[CACHE_SIZE].as<size_t> () * 1024 * 1024;
    Application(rendererFilename.c_str(), io, m_iReturnValue, cacheSize);
}

void Configuration::displayVersion() {
    cout << "Mikros Image Player - (C) Copyright 2007-2011 Mikros Image" << endl;
    cout << BUILD_INFORMATION;
#if defined DEBUG
    cout << " - DEBUG" << endl;
#else
    cout << " - RELEASE" << endl;
#endif
}

void Configuration::displayHelp() {
    cout << m_CmdlineOptionsGroup << endl;
}

