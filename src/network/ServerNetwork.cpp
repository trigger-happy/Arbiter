#include "ServerNetwork.h"

ServerNetwork::ServerNetwork(boost::asio::io_service& io_service, uint16_t port, std::string secret, uint64_t pingTime)
  : acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
  secret_ = secret;
  pingTime_ = pingTime;
  
  boost::shared_ptr<RunnerConnection> new_conn(new RunnerConnection(acceptor_.get_io_service(), secret_, pingTime_));
  acceptor_.async_accept(new_conn->socket(), boost::bind(&ServerNetwork::handleAccept, this, boost::asio::placeholders::error, new_conn));
}

ServerNetwork::~ServerNetwork()
{
  listeners_.clear();
}

void ServerNetwork::handleAccept(const boost::system::error_code& e, boost::shared_ptr< RunnerConnection > conn)
{
  if(!e) {
    conn->start();
    std::vector<ServerNetworkListener*>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
      (*iter)->runnerConnected(conn);
    }
    
    boost::shared_ptr<RunnerConnection> new_conn(new RunnerConnection(acceptor_.get_io_service(), secret_, pingTime_));
    acceptor_.async_accept(new_conn->socket(), boost::bind(&ServerNetwork::handleAccept, this, boost::asio::placeholders::error, new_conn));
  }
  else {
    std::vector<ServerNetworkListener*>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
      (*iter)->disconnected();
    }
  }
}

void ServerNetwork::addListener(ServerNetworkListener& listener)
{
  listeners_.push_back(&listener);
}
