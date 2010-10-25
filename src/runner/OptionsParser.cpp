#include "OptionsParser.h"

using namespace std;
using namespace boost::program_options;

namespace oh = boost::program_options;

//helper function specified in the boost library documentation
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v){
  copy(v.begin(), v.end(), ostream_iterator<T>(cout, " "));
  return os;
}

OptionsParser::OptionsParser() : 
  m_verbose(false),
  m_descriptions("Allowed Options"),
  m_server("127.0.0.1"),
  m_port("9999"),
  m_pwd("LOCAL/SUBDIRECTORY/PROBLEM/path"),
  m_input("lol.jpg"),
  m_version("000000000001"){
    try{
      
    //local variables that will hold the parsed values
    bool verbose;
    string version;
    string passcode;
    string server;
    string port;
    string pwd;
    string input;
    
//     m_descriptions = new options_description("Allowed Options");
    
    //declaring allowed options
    m_descriptions.add_options()
      ("help","We will do our best to help you")
      ("verbose,v", "Enable verbose mode")
      ("version",oh::value<string>(&version), "Prints the version id")
      ("options,o", "Lists all options in [name,value] pairs")
      ("passcode,p",oh::value<string>(&passcode)->default_value("12345") , 
	  "Sets the passcode")
      ("server,s",oh::value<string>(&server)->default_value("127.0.0.1") , 
	  "Sets the server address")
      ("port,p", oh::value<string>(&port)->default_value("127.0.0.1"),
	  "Sets the port's address")
      ("input-path,I", oh::value<string>(&pwd)->default_value("LOCAL/SUBDIRECTORY/PROBLEM/path"),
	  "Sets the current working directory")
      ("input-file", oh::value<string>(&input)->default_value("a.cpp"),
	  "Sets the file(s) to be sent");
    
    //filling up positional information
    m_position.add("input-file", -1);
    
  } catch(std::exception& e) {
    cout << e.what() << endl;
  }
}

OptionsParser::OptionsParser(const OptionsParser &options_h):
  m_verbose(options_h.m_verbose),
  m_server(options_h.m_server),
  m_port(options_h.m_port),
  m_pwd(options_h.m_pwd),
  m_input(options_h.m_input),
  m_version(options_h.m_version),
  m_position(options_h.m_position),
  m_variablemap(options_h.m_variablemap),
  m_descriptions(options_h.m_descriptions){}

/*one can set the options here, using boost/program_options' classes and functions
upon program execution, default values from the config file are read*/
void OptionsParser::setOptions(int elements, char** options){
  //storing and parsing at nearly the SAME TIME!
  store(command_line_parser(elements, options).
    options(m_descriptions).positional(m_position).run(), m_variablemap);
  notify(m_variablemap);
  
  if(m_variablemap.count("help")){
    listOptions();
  }
  if(m_variablemap.count("version")){
    cout << m_version<< endl;
  }
  if(m_variablemap.count("verbose")){
    setVerbose(true);
  }
  if(m_variablemap.count("options")){
    getOptions();
  }
  if(m_variablemap.count("passcode")){
    setPasscode(m_variablemap["passcode"].as<string>());
  }
  if(m_variablemap.count("server")){
    setServer(m_variablemap["server"].as<string>());
  }
  if(m_variablemap.count("port")){
    setPort(m_variablemap["port"].as<string>());
  }
  if(m_variablemap.count("input-path")){
    setDirectory(m_variablemap["input-path"].as<string>());
  }
  if(m_variablemap.count("input-file")){
    setInput(m_variablemap["input-file"].as<string>());
  }
}

/*lists in stdout the options available. This is called when --help*/
void OptionsParser::listOptions(){
  cout << "Usage: options_description [options]\n";
  cout << m_descriptions;
}

/*prints the [name,value] pairs that the program is running on using 
boost/program_options/variable_map*/
void OptionsParser::printOptions(){
  cout << "Here are the current [name,value] pairs\n";
  cout << "Server: " << m_variablemap["server"].as<string>() << endl;
  cout << "Passcode: " << m_variablemap["passcode"].as<string>() << endl;
  cout << "Port: " << m_variablemap["port"].as<string>() << endl;
  cout << "Version: " << m_version << endl;
  cout << "Input-path: " << m_variablemap["input-path"].as<string>() << endl;
}

/*gets the actual variable map that may be used by any other module ;)*/
oh::variables_map OptionsParser::getOptions(){
  return m_variablemap;
}

//sets/gets the verbose address
void OptionsParser::setVerbose(const bool &verbose){
    m_verbose = verbose;
}
bool OptionsParser::getVerbose(){
  return m_verbose;
}

//sets/gets the passcode
void OptionsParser::setPasscode(const string &passcode){
  m_passcode = passcode;
}
string OptionsParser::getPasscode(){
  return m_passcode;
}

//sets/gets the server address
void OptionsParser::setServer(const string &server){
  m_server = server;
}
string OptionsParser::getServer(){
  return m_server;
}

//sets/gets the server port
void OptionsParser::setPort(const string &port){
  m_port = port;
}
string OptionsParser::getPort(){
  return m_port;
}

//sets/gets the current working directory
void OptionsParser::setDirectory(const string &directory){
  m_pwd = directory;
}
string OptionsParser::getDirectory(){
  return m_pwd;
}

//sets/gets the input files; with the whole path
void OptionsParser::setInput(const string &files){
  m_input=files;
}
string OptionsParser::getInput(){
  return m_input;
}

