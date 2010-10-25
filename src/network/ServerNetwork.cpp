#include "ServerNetwork.h"

ServerNetwork::ServerNetwork(boost::asio::io_service& io_service, std::string secret, uint64_t pingTime)
  : acceptor_(io_service)
{
  secret_ = secret;
  pingTime_ = pingTime;
  
  boost::shared_ptr<RunnerConnection> new_conn(new RunnerConnection(acceptor_.get_io_service(), secret_, pingTime_));
  acceptor_.async_accept(new_conn->socket(), boost::bind(&ServerNetwork::handleAccept, this, boost::asio::placeholders::error, new_conn));
}

ServerNetwork::~ServerNetwork()
{
  if(acceptor_.is_open()) {
    acceptor_.close();
  }
}

void ServerNetwork::handleAccept(const boost::system::error_code& e, boost::shared_ptr< RunnerConnection > conn)
{
  if(!e) {
    conn->start();
    boost::ptr_vector<ServerNetworkListener>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
      iter->runnerConnected(conn);
    }
    
    boost::shared_ptr<RunnerConnection> new_conn(new RunnerConnection(acceptor_.get_io_service(), secret_, pingTime_));
    acceptor_.async_accept(new_conn->socket(), boost::bind(&ServerNetwork::handleAccept, this, boost::asio::placeholders::error, new_conn));
  }
  else {
    boost::ptr_vector<ServerNetworkListener>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
      iter->disconnected();
    }
  }
}

void ServerNetwork::addListener(ServerNetworkListener& listener)
{
  listeners_.push_back(&listener);
}
