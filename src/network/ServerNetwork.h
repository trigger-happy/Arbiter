#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include <string>
#include "RunnerConnection.h"

class ServerNetworkListener
{
public:
  virtual void runnerConnected(boost::shared_ptr<RunnerConnection>) = 0;
  virtual void disconnected() = 0;
};

class ServerNetwork
{
public:
  ServerNetwork(boost::asio::io_service& io_service, uint16_t port, std::string secret, uint64_t pingTime);
  virtual ~ServerNetwork();
  void addListener(ServerNetworkListener&);
  
private:  
  void handleAccept(const boost::system::error_code& e, boost::shared_ptr<RunnerConnection> conn);
  
  std::string secret_;
  uint64_t pingTime_;
  
  boost::asio::ip::tcp::acceptor acceptor_;
  
  boost::ptr_vector<ServerNetworkListener> listeners_;
};



#endif // SERVER_NETWORK_H
