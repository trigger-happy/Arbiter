#ifndef RUNNER_OPTIONSPARSER_H__
#define RUNNER_OPTIONSPARSER_H__

#include <boost/program_options.hpp>

#include <iostream>
#include <algorithm>
#include <iterator>

#include <string>

/**
  * Parses command-line options.
  */
class OptionsParser {
public:
	enum LogLevel { Verbose, Normal, Warnings, Errors, Quiet };
private:
  //flag for verbose mode
	bool m_verbose;
	LogLevel m_loglevel;

  //security string; some sort of passcode set at the beginning
  //of the main program
  std::string m_key;

  //string for the server address; format is something like 127.0.0.1
  std::string m_server;

  //string for the server port; format is something like 1971
  uint16_t m_port;

  uint64_t m_pingInterval;

  //string for the current working directory
  std::string m_pwd;

  //options descriptor for the semantic information
  boost::program_options::options_description m_descriptions;
  
  //variable map for the parsed variables
  boost::program_options::variables_map m_variablemap;

  public:
    //Constructors
    OptionsParser();

    //Copy Constructor
	OptionsParser(const OptionsParser &options_h);

    bool getVerbose();
	std::string getKey();
    std::string getServer();
	uint16_t getPort();
	uint64_t getPingInterval() { return m_pingInterval; }

    std::string getDirectory();
	bool isHelpToggled();
	bool isVersionToggled();


	void parseArgs(int argv, char **argc);
    void listOptions();
    boost::program_options::variables_map getOptions();
    void printOptions();
};


#endif
