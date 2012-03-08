#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <boost/utility.hpp>
#include <boost/program_options.hpp>
#include <dukexcore/dkxSession.h>

class Configuration : boost::noncopyable {
public:
    Configuration(Session::ptr);

public:
    bool parse(int argcc, char** argvv);
    void displayVersion();
    void displayHelp();

    boost::program_options::options_description m_CmdLineOnly;
    boost::program_options::options_description m_Config;
    boost::program_options::options_description m_Display;
    boost::program_options::options_description m_Interactive;
    boost::program_options::options_description m_CmdlineOptionsGroup;
    boost::program_options::options_description m_ConfigFileOptions;
    boost::program_options::options_description m_HiddenOptions;
    boost::program_options::variables_map       m_Vm;

private:
    Session::ptr mSession;
};

#endif /* CONFIGURATION_H_ */
