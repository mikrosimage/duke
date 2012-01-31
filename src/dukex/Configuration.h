#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <boost/utility.hpp>
#include <boost/program_options.hpp>

class Configuration {
public:
    Configuration(int argcc, char** argvv);

public:
    void displayVersion();
    void displayHelp();
    const bool stopped() const {
        return m_bStop;
    }
    const short port() const {
        return m_uPort;
    }
    const std::string path() const {
        return m_sPath;
    }

    boost::program_options::options_description m_CmdLineOnlyOptions;
    boost::program_options::options_description m_ConfigOptions;
    boost::program_options::options_description m_OptionGroup;
    boost::program_options::variables_map m_Vm;

private:
    bool m_bStop;
    short m_uPort;
    std::string m_sPath;
};

#endif /* CONFIGURATION_H_ */
