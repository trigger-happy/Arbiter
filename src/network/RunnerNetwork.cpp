#include "RunnerNetwork.h"
#include <polarssl/sha2.h>
#include <sstream>

std::string itoa(uint16_t num) {
  std::stringstream s;
  s << num;
  return s.str();
}

RunnerNetwork::RunnerNetwork(std::string address, uint16_t port, std::string secret, uint64_t pingTime)
  : io_service_(), connection_(new Connection(io_service_)), timer_(io_service_), pingTimer_(io_service_)
{
  address_ = address;
  port_ = port;
  secret_ = secret;
  pingTime_ = pingTime;
  connected_ = false;
  authenticated_ = false;
  retry_ = false;
  listener_ = 0;
}

RunnerNetwork::~RunnerNetwork()
{
}

void RunnerNetwork::start()
{
  do {
    boost::system::error_code e;

    // Resolve
    boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::resolver::query query(address_, itoa(port_));
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator;
    do {
      endpoint_iterator = resolver.resolve(query, e);
    } while(e);
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

    // Start an asynchronous connect operation.
    connection_->socket().async_connect(endpoint,
      boost::bind(&RunnerNetwork::handleConnect, this,
      boost::asio::placeholders::error, ++endpoint_iterator));
    io_service_.run();
    io_service_.reset();
  } while(retry_);
}

void RunnerNetwork::handleConnect(const boost::system::error_code& e, boost::asio::ip::basic_resolver_iterator< boost::asio::ip::tcp > endpoint_iterator)
{
  if (!e)
  {
    connected_ = true;
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
    retry_ = (listener_ && listener_->disconnected(e));
  }
}

void RunnerNetwork::handleReceive(const boost::system::error_code& e)
{
  if(!e) {
    timer_.cancel();
    if(inboundMessage_.type() == NetworkMessage::CHALLENGE) {
      std::string toHash = inboundMessage_.text() + connection_->socket().local_endpoint().address().to_string();
      unsigned char hash[32];
      sha2_hmac((unsigned char*)toHash.c_str(), toHash.length(),
        (unsigned char*)secret_.c_str(), secret_.length(),
        hash, false);
      std::string response((char*)hash, 32);
      outboundMessage_.set_type(NetworkMessage::CHALLENGE_RESPONSE);
      outboundMessage_.set_text(response);
      sendMessage();
    }
    else if(inboundMessage_.type() == NetworkMessage::ACKNOWLEDGE) {
      authenticated_ = true;
      if(listener_) listener_->authenticated();
      pingTimer_.expires_from_now(boost::posix_time::seconds(pingTime_));
      pingTimer_.async_wait(boost::bind(&RunnerNetwork::handlePing, this, boost::asio::placeholders::error()));
    }
    else if(authenticated_) {
      if(inboundMessage_.type() == NetworkMessage::PING_RESPONSE) {
	
      }
      else if(inboundMessage_.type() == NetworkMessage::PROBLEM_SET) {
        if(listener_) listener_->receiveProblemSet(inboundMessage_.problem_set());
      }
      else if(inboundMessage_.type() == NetworkMessage::LANGUAGE) {
        if(listener_) listener_->receiveLanguage(inboundMessage_.language());
      }
      else if(inboundMessage_.type() == NetworkMessage::RUN_ORDER) {
        if(listener_) listener_->receiveRunOrder(inboundMessage_.run_order());
      }
      else {
        boost::system::error_code ec(boost::asio::error::invalid_argument);
        disconnect(ec);
        return;
      }
      timer_.expires_from_now(boost::posix_time::seconds(pingTime_*2));
      timer_.async_wait(boost::bind(&RunnerNetwork::handleTimeout, this, boost::asio::placeholders::error));
    }
    else {
      boost::system::error_code ec(boost::asio::error::invalid_argument);
      disconnect(ec);
      return;
    }
    connection_->async_read(inboundMessage_, boost::bind(&RunnerNetwork::handleReceive, this, boost::asio::placeholders::error));
  }
  else if(e != boost::asio::error::operation_aborted) {
    disconnect(e);
  }
}

void RunnerNetwork::handleTimeout(const boost::system::error_code& e)
{
  if(!e) {
    boost::system::error_code ec(boost::asio::error::timed_out);
    disconnect(ec);
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

void RunnerNetwork::sendMessage() {
  connection_->async_write(outboundMessage_, boost::bind(&RunnerNetwork::handleSend, this, boost::asio::placeholders::error, connection_));
}

void RunnerNetwork::handleSend(const boost::system::error_code& e, connection_ptr conn)
{
  outboundMessage_.Clear();
  if(e && e != boost::asio::error::operation_aborted) {
    disconnect(e);
  }
}

void RunnerNetwork::stop()
{
  if(connected_) {
    pingTimer_.cancel();
    timer_.cancel();
    if(connection_->socket().is_open()) {
      connection_->socket().shutdown(boost::asio::socket_base::shutdown_both);
      connection_->socket().close();
    }
    connected_ = false;
    retry_ = false;
  }
}

void RunnerNetwork::disconnect(const boost::system::error_code& e)
{
  if(connected_) {
    stop();
    retry_ = (listener_ && listener_->disconnected(e));
  }
}

void RunnerNetwork::setListener(RunnerNetworkListener& listener)
{
  listener_ = &listener;
}

void RunnerNetwork::removeListener()
{
  listener_ = 0;
}
