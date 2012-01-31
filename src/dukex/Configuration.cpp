#include "Configuration.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "no information available - don't use in production"
#endif

const std::string HEADER = "[Configuration] ";
const char* PORT = "port";
const char* START = "start";

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

Configuration::Configuration(int argc, char** argv) :
    m_CmdLineOnlyOptions("info options"), m_ConfigOptions("configuration options"), m_bStop(false), m_uPort(7171), m_sPath("duke") {

    std::string configuration_filename;
    if (argc >= 1) {
        fs::path p(argv[0]);
        p.replace_extension(".ini");
        if (exists(p) && is_regular_file(p))
            configuration_filename = p.string();
    }

    // available on the command line
    m_CmdLineOnlyOptions.add_options() //
    ("help,h", "Displays this help") //
    ("version,v", "Displays the version informations");
    // available in the configuration file
    m_ConfigOptions.add_options() //
    (PORT, po::value<short>(), "Sets the port number to be used");
    m_ConfigOptions.add_options() //
    (START, po::value<string>(), "Automatically starts the 'duke' server hosted at the specified path");

    // parsing the command line
    m_OptionGroup.add(m_CmdLineOnlyOptions).add(m_ConfigOptions);
    po::store(po::parse_command_line(argc, argv, m_OptionGroup), m_Vm);

    // now parsing the configuration file - already stored variables will remain unchanged
    ifstream configFile(configuration_filename.c_str(), ifstream::in);
    if (configFile.is_open()) {
        cout << HEADER + "Reading options from " << configuration_filename << endl;
        po::store(po::parse_config_file(configFile, m_ConfigOptions), m_Vm);
    }
    configFile.close();

    // notifying from incoming new options
    po::notify(m_Vm);

    if (m_Vm.count("help")) {
        displayHelp();
        m_bStop = true;
    }

    if (m_Vm.count("version")) {
        displayVersion();
        m_bStop = true;
    }

    if (m_Vm.count(PORT)) {
        m_uPort = m_Vm[PORT].as<short> ();
    }

    if (m_Vm.count(START)) {
        m_sPath = m_Vm[START].as<string> ();
    }
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
    cout << m_OptionGroup << endl;
}

