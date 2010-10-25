#ifndef RUNNER_NETWORK_H
#define RUNNER_NETWORK_H

#include <string>
#include <boost/cstdint.hpp>
#include "Message.pb.h"
#include "Connection.h"

class RunnerNetworkListener
{
public:
  virtual void connected() {}
  virtual void disconnected() {}  
  virtual void receiveRunOrder(RunOrder&) = 0;
  virtual void receiveProblemSet(ProblemSet&) = 0;
  virtual void receiveLanguage(Language&) = 0;
};

class RunnerNetwork
{
public:
  RunnerNetwork(boost::asio::io_service& io_service)
    : connection_(new Connection(io_service))
  {
  }
  
  virtual ~RunnerNetwork()
  {
  }
  
  void start();
  void sendMessage(Message& message);
  void requestProblemSet(boost::uint32_t problem_id);
  void sendRunResult(boost::uint32_t run_id, boost::uint32_t result);
  void stop();
  
private:
  connection_ptr connection_;
};

#endif // RUNNER_NETWORK_H
