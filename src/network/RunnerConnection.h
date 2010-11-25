#ifndef RUNNERCONNECTION_H
#define RUNNERCONNECTION_H

#include <vector>
#include <boost/cstdint.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "Connection.h"
#include "Message.pb.h"
#include <boost/concept_check.hpp>

class RunnerConnectionListener
{
public:
  virtual void authenticated() {}
  virtual void disconnected() {}
  virtual void receiveProblemSetRequest(std::string pid) = 0;
  virtual void receiveLanguageRequest(boost::uint32_t lid) = 0;
  virtual void receiveRunResult(RunResult) = 0;
};

class RunnerConnection
{
public:
  RunnerConnection(boost::asio::io_service& io_service, std::string secret, uint64_t pingTime);
  virtual ~RunnerConnection();
  boost::asio::ip::tcp::socket& socket();
  void start();
  void sendMessage();
  void sendRunOrder(uint32_t run_id, std::string problem_id, std::string problem_hash, uint32_t language_id, std::string language_hash, std::string attachment);  
  void sendProblemSet(std::string problem_id, std::string problem_hash, std::string attachment);  
  void sendLanguage(boost::uint32_t language_id, std::string language_hash, std::string attachment);
  void setListener(RunnerConnectionListener& rcl);
  void removeListener();
  
private:
  void handleSend(const boost::system::error_code& e, connection_ptr conn);
  void handleReceive(const boost::system::error_code& e);
  void handleTimeout(const boost::system::error_code& e);
  void disconnect();
  
  connection_ptr connection_;  
  boost::asio::deadline_timer timer_;
  std::string secret_;
  uint64_t pingTime_;
  
  std::string address_;  
  NetworkMessage outboundMessage_;
  NetworkMessage inboundMessage_;
  bool authenticated_;
  std::string challenge_; 
  
  RunnerConnectionListener* listener_;
};

#endif // RUNNERCONNECTION_H
