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
  virtual void authenticated() = 0;
  virtual bool disconnected(const boost::system::error_code& ec) = 0;
  virtual void receiveRunOrder(RunOrder) = 0;
  virtual void receiveProblemSet(ProblemSet) = 0;
  virtual void receiveLanguage(Language) = 0;
};

class RunnerNetwork
{
public:
  RunnerNetwork(std::string address, uint16_t port, std::string secret, uint64_t pingTime);
  virtual ~RunnerNetwork();
  void start();
  void stop();
  void requestProblemSet(std::string problem_id);
  void requestLanguage(boost::uint32_t language_id);
  void sendRunResult(boost::uint32_t run_id, boost::uint32_t result, std::string team_stdout, std::string team_stderr, boost::uint64_t run_time);
  void setListener(RunnerNetworkListener& listener);
  void removeListener();

private:
  void handleConnect(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
  void handleSend(const boost::system::error_code& e, connection_ptr conn);
  void handleReceive(const boost::system::error_code& e);
  void handleTimeout(const boost::system::error_code& e);
  void handlePing(const boost::system::error_code& e);
  void sendMessage();
  bool authenticate();
  void disconnect(const boost::system::error_code& e);

  std::string address_;
  uint16_t port_;
  std::string secret_;
  uint64_t pingTime_;

  boost::asio::io_service io_service_;
  connection_ptr connection_;
  boost::asio::deadline_timer timer_;
  boost::asio::deadline_timer pingTimer_;
  NetworkMessage inboundMessage_;
  NetworkMessage outboundMessage_;

  bool retry_;
  bool connected_;
  bool authenticated_;

  RunnerNetworkListener* listener_;
};

#endif // RUNNER_NETWORK_H
