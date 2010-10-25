#include "RunnerNetwork.h"
#include <polarssl/sha2.h>
#include <sstream>

std::string itoa(uint16_t num) {
  std::stringstream s;
  s << num;
  return s.str();  
}

RunnerNetwork::RunnerNetwork(boost::asio::io_service& io_service, std::string address, uint16_t port, uint64_t pingTime)
  : connection_(new Connection(io_service)), timer_(io_service), pingTimer_(io_service)
{
  pingTime_ = pingTime;
  authenticated_ = false;
  
  // Resolve the host name into an IP address.
  boost::asio::ip::tcp::resolver resolver(io_service);
  boost::asio::ip::tcp::resolver::query query(address, itoa(port));
  boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
    resolver.resolve(query);
  boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

  // Start an asynchronous connect operation.
  connection_->socket().async_connect(endpoint,
      boost::bind(&RunnerNetwork::handleConnect, this,
	boost::asio::placeholders::error, ++endpoint_iterator));
}

RunnerNetwork::~RunnerNetwork()
{
  listeners_.clear();
}

void RunnerNetwork::handleConnect(const boost::system::error_code& e, boost::asio::ip::basic_resolver_iterator< boost::asio::ip::tcp > endpoint_iterator)
{
  if (!e)
  {
    std::vector<RunnerNetworkListener*>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
      (*iter)->connected();
    }
    outboundMessage_.set_type(NetworkMessage::CONNECT);
    sendMessage();
    connection_->async_read(inboundMessage_,
	boost::bind(&RunnerNetwork::handleReceive, this,
	  boost::asio::placeholders::error));
  }
  else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
  {
    // Try the next endpoint.
    connection_->socket().close();
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
    connection_->socket().async_connect(endpoint,
	boost::bind(&RunnerNetwork::handleConnect, this,
	  boost::asio::placeholders::error, ++endpoint_iterator));
  }
  else
  {
    std::vector<RunnerNetworkListener*>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
      (*iter)->connectionFailed();
    }
  }
}

void RunnerNetwork::handleReceive(const boost::system::error_code& e)
{
  if(!e) {
    timer_.cancel();
    if(inboundMessage_.type() == NetworkMessage::CHALLENGE) {
      std::string original = connection_->socket().local_endpoint().address().to_string() + inboundMessage_.text();
      unsigned char hash[32];
      sha2(reinterpret_cast<const unsigned char*>(original.c_str()), original.length(), &hash[0], false);
      std::string response(reinterpret_cast<char*>(hash));
      outboundMessage_.set_type(NetworkMessage::CHALLENGE_RESPONSE);
      outboundMessage_.set_text(response);
      sendMessage();
    }
    else if(inboundMessage_.type() == NetworkMessage::ACKNOWLEDGE) {
      authenticated_ = true;
      std::vector<RunnerNetworkListener*>::iterator iter;
      for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	(*iter)->authenticated();
      }
      pingTimer_.expires_from_now(boost::posix_time::seconds(pingTime_));
      pingTimer_.async_wait(boost::bind(&RunnerNetwork::handlePing, this, boost::asio::placeholders::error()));
    }
    else if(authenticated_) {
      if(inboundMessage_.type() == NetworkMessage::PING_RESPONSE) {
	
      }
      else if(inboundMessage_.type() == NetworkMessage::PROBLEM_SET) {
	std::vector<RunnerNetworkListener*>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  (*iter)->receiveProblemSet(inboundMessage_.problem_set());
	}
      }
      else if(inboundMessage_.type() == NetworkMessage::LANGUAGE) {
	std::vector<RunnerNetworkListener*>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  (*iter)->receiveLanguage(inboundMessage_.language());
	}
      }
      else if(inboundMessage_.type() == NetworkMessage::RUN_ORDER) {
	std::vector<RunnerNetworkListener*>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  (*iter)->receiveRunOrder(inboundMessage_.run_order());
	}
      }
      else {
	disconnect();
	return;
      }
      timer_.expires_from_now(boost::posix_time::seconds(pingTime_*2));
      timer_.async_wait(boost::bind(&RunnerNetwork::handleTimeout, this, boost::asio::placeholders::error));
    }
    else {
      disconnect();
      return;
    }
    connection_->async_read(inboundMessage_, boost::bind(&RunnerNetwork::handleReceive, this, boost::asio::placeholders::error));
  }
  else if(e != boost::asio::error::operation_aborted) {
    disconnect();
  }
}

void RunnerNetwork::handleTimeout(const boost::system::error_code& e)
{
  if(!e) {
    disconnect();
  }
}

void RunnerNetwork::handlePing(const boost::system::error_code& e)
{
  if(!e) {
    outboundMessage_.set_type(NetworkMessage::PING);
    sendMessage();
    pingTimer_.expires_from_now(boost::posix_time::seconds(pingTime_));
    pingTimer_.async_wait(boost::bind(&RunnerNetwork::handlePing, this, boost::asio::placeholders::error));
  }
}


void RunnerNetwork::requestLanguage(uint32_t language_id)
{
  outboundMessage_.set_type(NetworkMessage::LANGUAGE_REQUEST);
  outboundMessage_.set_requested_item_id(language_id);
  sendMessage();
}

void RunnerNetwork::requestProblemSet(std::string problem_id)
{
  outboundMessage_.set_type(NetworkMessage::PROBLEM_SET_REQUEST);
  outboundMessage_.set_text(problem_id);
  sendMessage();
}

void RunnerNetwork::sendRunResult(uint32_t run_id, uint32_t result, std::string team_stdout, std::string team_stderr, uint64_t run_time)
{
  outboundMessage_.set_type(NetworkMessage::RUN_RESULT);
  RunResult* rr = outboundMessage_.mutable_run_result();
  rr->set_run_id(run_id);
  rr->set_result(result);
  rr->set_team_stdout(team_stdout);
  rr->set_team_stderr(team_stderr);
  rr->set_run_time(run_time);
  sendMessage();
}

void RunnerNetwork::sendMessage()
{
  connection_->async_write(outboundMessage_, boost::bind(&RunnerNetwork::handleSend, this, boost::asio::placeholders::error, connection_));
}

void RunnerNetwork::handleSend(const boost::system::error_code& e, connection_ptr conn)
{
  outboundMessage_.Clear();
  if(e && e != boost::asio::error::operation_aborted) {
    disconnect();
  }
}

void RunnerNetwork::disconnect()
{
  pingTimer_.cancel();
  timer_.cancel();
  if(connection_->socket().is_open()) {
    connection_->socket().shutdown(boost::asio::socket_base::shutdown_both);
    connection_->socket().close();
  }
  
  std::vector<RunnerNetworkListener*>::iterator iter;
  for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
    (*iter)->disconnected();
  }
}

void RunnerNetwork::addListener(RunnerNetworkListener& listener)
{
  listeners_.push_back(&listener);
}

