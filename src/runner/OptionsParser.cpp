#include "OptionsParser.h"

using namespace std;
using namespace boost::program_options;

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
  m_port(9999),
  m_pwd("") {
	try {

		//declaring allowed options
		m_descriptions.add_options()
				("help","Show this message.")
				("version", "Print out version information.")
				("verbose,v", value<bool>(&m_verbose)->default_value(false), "Enable verbose mode.")
				("passcode,p",value<string>(&m_passcode)->default_value("fckgw-rhqq2-yxrkt-8tg6w-2b7q8"),
				 "Sets the passcode.")
				("server,s",value<string>(&m_server)->default_value("127.0.0.1") ,
				 "Sets the server address.")
				("port,p", value<uint16_t>(&m_port)->default_value(9999),
				 "Sets the port's address.")
				("directory,D", value<string>(&m_pwd),
				 "Sets the current working directory.");
	} catch(std::exception& e) {
		cout << e.what() << endl;
	}
}

OptionsParser::OptionsParser(const OptionsParser &options_h):
  m_verbose(options_h.m_verbose),
  m_server(options_h.m_server),
  m_port(options_h.m_port),
  m_pwd(options_h.m_pwd),
  m_variablemap(options_h.m_variablemap),
  m_descriptions(options_h.m_descriptions){}

/*one can set the options here, using boost/program_options' classes and functions
upon program execution, default values from the config file are read*/
void OptionsParser::parseArgs(int argc, char **argv) {
  store(parse_command_line(argc, argv, m_descriptions), m_variablemap);
  notify(m_variablemap);
}

bool OptionsParser::isHelpToggled() {
	return m_variablemap.count("help") > 0;
}
bool OptionsParser::isVersionToggled() {
	return m_variablemap.count("version") > 0;
}

/*lists in stdout the options available. This is called when --help*/
void OptionsParser::listOptions(){
  cout << m_descriptions;
}

/*prints the [name,value] pairs that the program is running on using 
boost/program_options/variable_map*/
void OptionsParser::printOptions(){
  cout << "Here are the current [name,value] pairs\n";
  cout << "Server: " << m_variablemap["server"].as<string>() << endl;
  cout << "Passcode: " << m_variablemap["passcode"].as<string>() << endl;
  cout << "Port: " << m_variablemap["port"].as<uint16_t>() << endl;
  cout << "Directory: " << m_variablemap["directory"].as<string>() << endl;
}

/*gets the actual variable map that may be used by any other module ;)*/
variables_map OptionsParser::getOptions(){
  return m_variablemap;
}

bool OptionsParser::getVerbose(){
  return m_verbose;
}

string OptionsParser::getPasscode(){
  return m_passcode;
}

string OptionsParser::getServer(){
  return m_server;
}

uint16_t OptionsParser::getPort(){
  return m_port;
}

string OptionsParser::getDirectory(){
  return m_pwd;
}


