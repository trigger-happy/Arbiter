#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include <string>
#include "RunnerConnection.h"

class ServerNetworkListener
{
public:
  virtual void runnerConnected(boost::shared_ptr<RunnerConnection>) = 0;
  virtual void disconnected(const boost::system::error_code& ec) = 0;
};

class ServerNetwork
{
public:
  ServerNetwork(uint16_t port, std::string secret, uint64_t pingTime);
  virtual ~ServerNetwork();
  void start();
  void stop();
  void setListener(ServerNetworkListener&);
  void removeListener();
  
private:  
  void handleAccept(const boost::system::error_code& e, boost::shared_ptr<RunnerConnection> conn);
  
  std::string secret_;
  uint64_t pingTime_;
  
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  
  ServerNetworkListener* listener_;
};



#endif // SERVER_NETWORK_H
