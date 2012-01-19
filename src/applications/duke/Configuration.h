#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <boost/utility.hpp>
#include <boost/program_options.hpp>

//forward declaration
struct IMessageIO;

/**
 * Class to handle command line arguments, configuration files
 * and configuration of the application.
 */
class Configuration : boost::noncopyable
{
public:
    Configuration( int argcc, char** argvv );
    int m_iReturnValue;

private: Configuration();
    Configuration( Configuration& );

    void displayVersion();
    void displayHelp();

    void run( IMessageIO& io);
    void decorateAndRun( IMessageIO& io);

    boost::program_options::options_description m_CmdLineOnly;
    boost::program_options::options_description m_Config;
    boost::program_options::options_description m_Display;
    boost::program_options::options_description m_Interactive;
    boost::program_options::options_description m_CmdlineOptionsGroup;
    boost::program_options::options_description m_ConfigFileOptions;
    boost::program_options::options_description m_HiddenOptions;
    boost::program_options::variables_map       m_Vm;

};

#endif /* CONFIGURATION_H_ */
