/*
	Server object
	
	copyright (c) 2010 Nikolai Andrei E. Banasihan

*/

// user headers
#include "Server.h"

// Boost
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>
using namespace boost;

// C++
#include<iostream>
using namespace std;

Server::Server() {
}


Server::~Server() {
}

void
Server::init() {

	cout << "enter init" << endl;
	// initialize objects
	
	
	/*
		And then, once the initialization has been made,
		start the thread.
	
	*/
	thread my_thread(run_thread);
	
	my_thread.join();
}

void
Server::run_thread() {
	int x = 0;
	while(x < 20) {
		cout << "Run Thread: " << x << endl;
		++x;
	}
	return;
}
