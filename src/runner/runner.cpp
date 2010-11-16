#include <iostream>
#include <apr.h>
#include <apr_general.h>
#include "OptionsParser.h"
#include "../src/network/RunnerNetwork.h"

#define VERSION_STRING "0.0.1"

using namespace std;

int main(int argc, char **argv) {
	apr_app_initialize(&argc, (const char * const **)&argv, 0);
	atexit(apr_terminate);

	OptionsParser handler;
	try{
		handler.parseArgs(argc,argv);
		if ( handler.isHelpToggled() ) {
			cout << "Usage: " << argv[0] << " [options]\n";
			handler.listOptions();
			return 0;
		} else if ( handler.isVersionToggled() ) {
			cout << VERSION_STRING << endl;
		}
		cout << endl;
		handler.printOptions();
	} catch(std::exception& e) {
		cout << e.what() << endl;
		return 1;
	}


	return 0;

}
