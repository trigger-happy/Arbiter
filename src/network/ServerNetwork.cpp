#include "ServerNetwork.h"

ServerNetwork::ServerNetwork(uint16_t port, std::string secret, uint64_t pingTime)
  : io_service_(), acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
  secret_ = secret;
  pingTime_ = pingTime;
  listener_ = 0;
}

ServerNetwork::~ServerNetwork()
{
}

void ServerNetwork::start() {
  boost::shared_ptr<RunnerConnection> new_conn(new RunnerConnection(io_service_, secret_, pingTime_));
  acceptor_.async_accept(new_conn->socket(), boost::bind(&ServerNetwork::handleAccept, this, boost::asio::placeholders::error, new_conn));
  io_service_.run();
  io_service_.reset();
}

void ServerNetwork::handleAccept(const boost::system::error_code& e, boost::shared_ptr< RunnerConnection > conn)
{
  if(!e) {
    conn->start();
    if(listener_) listener_->runnerConnected(conn);
    boost::shared_ptr<RunnerConnection> new_conn(new RunnerConnection(acceptor_.get_io_service(), secret_, pingTime_));
    acceptor_.async_accept(new_conn->socket(), boost::bind(&ServerNetwork::handleAccept, this, boost::asio::placeholders::error, new_conn));
  }
  else {
    if(listener_) listener_->disconnected(e);
  }
}

void ServerNetwork::stop() {
  acceptor_.cancel();
}

void ServerNetwork::setListener(ServerNetworkListener& listener)
{
  listener_ = &listener;
}

void ServerNetwork::removeListener()
{
  listener_ = 0;
}
