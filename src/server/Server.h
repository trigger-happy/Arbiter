#ifndef SERVER_H
#define SERVER_H


class Server {
	public:
		Server();
		~Server();
		
		void init();
		
		static void run_thread();
		
};



#endif // SERVER_H

