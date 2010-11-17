#ifndef RUNNER_NETWORK_H
#define RUNNER_NETWORK_H

#include <string>
#include <boost/cstdint.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "Message.pb.h"
#include "Connection.h"

class RunnerNetworkListener
{
public:
  virtual void connected() {}
  virtual void authenticated() {}
  virtual void disconnected() {}  
  virtual void connectionFailed() = 0;
  virtual void receiveRunOrder(RunOrder) = 0;
  virtual void receiveProblemSet(ProblemSet) = 0;
  virtual void receiveLanguage(Language) = 0;
};

class RunnerNetwork
{
public:
  RunnerNetwork(boost::asio::io_service& io_service, std::string address, uint16_t port, uint64_t pingTime);  
  virtual ~RunnerNetwork();
  void sendMessage();
  void requestProblemSet(std::string problem_id);
  void requestLanguage(boost::uint32_t language_id);
  void sendRunResult(boost::uint32_t run_id, boost::uint32_t result, std::string team_stdout, std::string team_stderr, boost::uint64_t run_time);
  void addListener(RunnerNetworkListener& listener);
  void removeListener(RunnerNetworkListener& listener);
  
private:
  void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
  void handleSend(const boost::system::error_code& e, connection_ptr conn);
  void handleReceive(const boost::system::error_code& e);
  void handleTimeout(const boost::system::error_code& e);
  void handlePing(const boost::system::error_code& e);
  void disconnect();
  
  uint64_t pingTime_;
  
  connection_ptr connection_;
  boost::asio::deadline_timer timer_;
  boost::asio::deadline_timer pingTimer_;
  NetworkMessage inboundMessage_;
  NetworkMessage outboundMessage_;
  
  bool authenticated_;
  
  std::vector<RunnerNetworkListener*> listeners_;
};

#endif // RUNNER_NETWORK_H
