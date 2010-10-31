#ifndef SERVER_H
#define SERVER_H

#include<string>

class Server {
	public:
		Server();
		~Server();
		
		void init();
		
		static void run_thread();
		
		enum ResponseType {SUCCESS, WRONG_PASSWORD, NO_SUCH_USER};
	
	private:
		// functions for getting the login information
		void connectLogin();
		void onLogin(std::string username, std::string password);
		
};



#endif // SERVER_H

