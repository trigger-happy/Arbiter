#include "RunnerConnection.h"
#include <polarssl/sha2.h>

void RunnerConnection::start()
{
  address_ = connection_->socket().remote_endpoint().address().to_string();
  connection_->async_read(inboundMessage_, boost::bind(&RunnerConnection::handleReceive, this, boost::asio::placeholders::error));
}

void RunnerConnection::handleReceive(const boost::system::error_code& e)
{
  if(!e) {
    if(inboundMessage_.type() == Message::CONNECT) {
      outboundMessage_.set_type(Message::CHALLENGE);
      outboundMessage_.set_text(challenge_);
      sendMessage(outboundMessage_);
    }
    else if(inboundMessage_.type() == Message::CHALLENGE_RESPONSE) {
      std::string expected = address_ + challenge_;
      unsigned char hash[32];
      sha2(reinterpret_cast<const unsigned char*>(expected.c_str()), expected.length(), &hash[0], false);
      if(inboundMessage_.text().compare(reinterpret_cast<char*>(hash))) {
	authenticated_ = true;
	outboundMessage_.set_type(Message::ACKNOWLEDGE);
      }
    }
    else if(authenticated_) {
      hasPinged_ = true;
      if(inboundMessage_.type() == Message::PING) {
	outboundMessage_.set_type(Message::PING_RESPONSE);
	sendMessage(outboundMessage_);
      }
      else if(inboundMessage_.type() == Message::PROBLEM_SET_REQUEST) {
	boost::ptr_vector<RunnerConnectionListener>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  iter->receiveProblemSetRequest(inboundMessage_.requested_item_id());
	}
      }
      else if(inboundMessage_.type() == Message::LANGUAGE_REQUEST) {
	boost::ptr_vector<RunnerConnectionListener>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  iter->receiveLanguageRequest(inboundMessage_.requested_item_id());
	}
      }
      else if(inboundMessage_.type() == Message::RUN_RESULT) {
	boost::ptr_vector<RunnerConnectionListener>::iterator iter;
	for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
	  iter->receiveRunResult(inboundMessage_.run_result());
	}
      }
    }
    else {
      disconnect();
    }
    connection_->async_read(inboundMessage_, boost::bind(&RunnerConnection::handleReceive, this, boost::asio::placeholders::error));
  }
  else {
    disconnect();
  }
}

void RunnerConnection::disconnect() {
  boost::ptr_vector<RunnerConnectionListener>::iterator iter;
  for(iter = listeners_.begin(); iter != listeners_.end(); iter++) {
    iter->disconnected();
  }
}

void RunnerConnection::sendProblemSet(uint32_t problem_id, std::string attachment)
{
  outboundMessage_.set_type(Message::PROBLEM_SET);
  ProblemSet* ps = outboundMessage_.mutable_problem_set();
  ps->set_problem_id(problem_id);
  ps->set_attachment(attachment);
  sendMessage(outboundMessage_);
}

void RunnerConnection::sendRunOrder(uint32_t run_id, uint32_t problem_id, uint32_t language_id, std::string attachment)
{
  outboundMessage_.set_type(Message::RUN_ORDER);
  RunOrder* ro = outboundMessage_.mutable_run_order();
  ro->set_run_id(run_id);
  ro->set_problem_id(problem_id);
  ro->set_language_id(language_id);
  ro->set_attachment(attachment);
  sendMessage(outboundMessage_);
}

void RunnerConnection::sendMessage(Message& message)
{
  connection_->async_write(message, boost::bind(&RunnerConnection::handleSend, this, boost::asio::placeholders::error, connection_));
}

void RunnerConnection::handleSend(const boost::system::error_code& e, connection_ptr conn)
{
  outboundMessage_.Clear();
  // TODO report error
}

void RunnerConnection::addListener(RunnerConnectionListener& rcl)
{
  listeners_.push_back(&rcl);
}

