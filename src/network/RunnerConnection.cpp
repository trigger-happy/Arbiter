#include "RunnerConnection.h"
#include <polarssl/sha2.h>

RunnerConnection::RunnerConnection(boost::asio::io_service& io_service, std::string secret, uint64_t pingTime)
  : connection_(new Connection(io_service)), timer_(io_service)
{
  authenticated_ = false;
  secret_ = secret;
  pingTime_ = pingTime;
}

RunnerConnection::~RunnerConnection()
{
  listeners_.clear();
}

boost::asio::ip::tcp::socket& RunnerConnection::socket()
{
  return connection_->socket();
}

void RunnerConnection::start()
{
  address_ = connection_->socket().remote_endpoint().address().to_string();
  connection_->async_read(inboundMessage_, boost::bind(&RunnerConnection::handleReceive, this, boost::asio::placeholders::error));
}

void RunnerConnection::handleTimeout(const boost::system::error_code& e)
{
  if(!e) {
    disconnect();
  }
}

void RunnerConnection::handleReceive(const boost::system::error_code& e)
{
  if(!e) {
    timer_.cancel();
    if(inboundMessage_.type() == NetworkMessage::CONNECT) {
      // generate challenge
      char randomString[65];
      static const char alphanum[] =
	    "0123456789"
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	    "abcdefghijklmnopqrstuvwxyz";

      for (int i = 0; i < 63; ++i) {
	  randomString[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
      }
      randomString[64] = 0;
      challenge_.assign(randomString);
      outboundMessage_.set_type(NetworkMessage::CHALLENGE);
      outboundMessage_.set_text(challenge_);
      sendMessage();
    }
    else if(inboundMessage_.type() == NetworkMessage::CHALLENGE_RESPONSE) {
      unsigned char hash[32];
      sha2_hmac((unsigned char*)challenge_.c_str(), challenge_.length(),
		(unsigned char*)address_.c_str(), address_.length(),
		hash, false);
      std::string hashstr((char*)hash, 32);
      if(inboundMessage_.text() == hashstr) {
	authenticated_ = true;
	outboundMessage_.set_type(NetworkMessage::ACKNOWLEDGE);
	sendMessage();
	std::vector<RunnerConnectionListener*>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  (*iter)->authenticated();
	}
      }
    }
    else if(authenticated_) {
      if(inboundMessage_.type() == NetworkMessage::PING) {
	outboundMessage_.set_type(NetworkMessage::PING_RESPONSE);
	sendMessage();
      }
      else if(inboundMessage_.type() == NetworkMessage::PROBLEM_SET_REQUEST) {
	std::vector<RunnerConnectionListener*>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  (*iter)->receiveProblemSetRequest(inboundMessage_.text());
	}
      }
      else if(inboundMessage_.type() == NetworkMessage::LANGUAGE_REQUEST) {
	std::vector<RunnerConnectionListener*>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  (*iter)->receiveLanguageRequest(inboundMessage_.requested_item_id());
	}
      }
      else if(inboundMessage_.type() == NetworkMessage::RUN_RESULT) {
	std::vector<RunnerConnectionListener*>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  (*iter)->receiveRunResult(inboundMessage_.run_result());
	}
      }
      else {
	disconnect();
	return;
      }
      timer_.expires_from_now(boost::posix_time::seconds(pingTime_*2));
      timer_.async_wait(boost::bind(&RunnerConnection::handleTimeout, this, boost::asio::placeholders::error));
    }
    else {
      disconnect();
      return;
    }    
    connection_->async_read(inboundMessage_, boost::bind(&RunnerConnection::handleReceive, this, boost::asio::placeholders::error));
  }
  else if(e != boost::asio::error::operation_aborted) {
    disconnect();
  }
}

void RunnerConnection::disconnect() {
  timer_.cancel();
  if(connection_->socket().is_open()) {
    connection_->socket().shutdown(boost::asio::socket_base::shutdown_both);
    connection_->socket().close();
  }
  std::vector<RunnerConnectionListener*>::iterator iter;
  for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
    (*iter)->disconnected();
  }
}

void RunnerConnection::sendProblemSet(std::string problem_id, std::string problem_hash, std::string attachment)
{
  outboundMessage_.set_type(NetworkMessage::PROBLEM_SET);
  ProblemSet* ps = outboundMessage_.mutable_problem_set();
  ps->set_problem_id(problem_id);
  ps->set_problem_hash(problem_hash);
  ps->set_attachment(attachment);
  sendMessage();
}

void RunnerConnection::sendLanguage(uint32_t language_id, std::string language_hash, std::string attachment)
{
  outboundMessage_.set_type(NetworkMessage::LANGUAGE);
  Language* l = outboundMessage_.mutable_language();
  l->set_language_id(language_id);
  l->set_language_hash(language_hash);
  l->set_attachment(attachment);
  sendMessage();
}


void RunnerConnection::sendRunOrder(uint32_t run_id, std::string problem_id, std::string problem_hash, uint32_t language_id, std::string language_hash, std::string attachment)
{
  outboundMessage_.set_type(NetworkMessage::RUN_ORDER);
  RunOrder* ro = outboundMessage_.mutable_run_order();
  ro->set_run_id(run_id);
  ro->set_problem_id(problem_id);
  ro->set_problem_hash(problem_hash);
  ro->set_language_id(language_id);
  ro->set_language_hash(language_hash);
  ro->set_attachment(attachment);
  sendMessage();
}

void RunnerConnection::sendMessage()
{
  connection_->async_write(outboundMessage_, boost::bind(&RunnerConnection::handleSend, this, boost::asio::placeholders::error, connection_));
}

void RunnerConnection::handleSend(const boost::system::error_code& e, connection_ptr conn)
{
  outboundMessage_.Clear();
  if(e && e != boost::asio::error::operation_aborted) {
    disconnect();
  }
}


void RunnerConnection::addListener(RunnerConnectionListener& rcl)
{
  listeners_.push_back(&rcl);
}

