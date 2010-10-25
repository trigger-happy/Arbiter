#include <iostream>
#include <apr.h>
#include <apr_general.h>
#include "OptionsParser.h"

using namespace std;

int main(int argc, char **argv) {
	apr_app_initialize(&argc, (const char * const **)&argv, 0);
	atexit(apr_terminate);

	try{
		OptionsParser* handler = new OptionsParser();
		handler->setOptions(argc,argv);
		cout << endl;
		handler->printOptions();
	} catch(std::exception& e) {
		cout << e.what() << endl;
		return 1;
	}
	return 0;
}
