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
  //flag for verbose mode
  bool m_verbose;

  //security string; some sort of passcode set at the beginning
  //of the main program
  std::string m_passcode;

  //string for the server address; format is something like 127.0.0.1
  std::string m_server;

  //string for the server port; format is something like 1971
  std::string m_port;

  //string for the current working directory
  std::string m_pwd;

  //string for the input-files
  std::string m_input;
  
  //string for the version id
  std::string m_version;

  //options descriptor for the semantic information
  boost::program_options::options_description m_descriptions;
  
  //positional information for syntactic information
  boost::program_options::positional_options_description m_position;
  
  //variable map for the parsed variables
  boost::program_options::variables_map m_variablemap;

  public:
    //Constructors
    OptionsParser();

    //Copy Constructor
	OptionsParser(const OptionsParser &options_h);
    
    //sets/gets the verbose address
    void setVerbose(const bool &verbose);
    bool getVerbose();
    
    //sets/gets the passcode
    void setPasscode(const std::string &passcode);
    std::string getPasscode();
    
    //sets/gets the server address
    void setServer(const std::string &server);
    std::string getServer();
    
    //sets/gets the server port
    void setPort(const std::string &port);
    std::string getPort();
    
    //sets/gets the current working directory
    void setDirectory(const std::string &directory);
    std::string getDirectory();
    
    //sets/gets the input files; with the whole path
    void setInput(const std::string &files);
    std::string getInput();

    void setOptions(int elements, char ** options);
    void listOptions();
    boost::program_options::variables_map getOptions();
    void printOptions();
};


#endif
