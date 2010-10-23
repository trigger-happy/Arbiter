#include "Server.h"

// C++
#include<iostream>


using namespace std;


int main(int argc, char **argv) {
	Server *server = new Server();
	
	server->init();

	return 0;
}
