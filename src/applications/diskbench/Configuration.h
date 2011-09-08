#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <boost/utility.hpp>
#include <boost/program_options.hpp>

/**
 * Class to handle command line arguments, configuration files
 * and configuration of the application.
 */
class Configuration : boost::noncopyable
{
public: Configuration( int argcc, char** argvv );
	int m_iReturnValue;

private: Configuration();
	Configuration( Configuration& );
	void displayVersion();
	void displayHelp();
	boost::program_options::options_description m_Generic;
	boost::program_options::variables_map m_Vm;
};

#endif /* CONFIGURATION_H_ */
