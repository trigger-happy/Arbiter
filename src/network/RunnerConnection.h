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
  virtual void disconnected() {}
  virtual void receiveProblemSetRequest(boost::uint32_t pid) = 0;
  virtual void receiveLanguageRequest(boost::uint32_t lid) = 0;
  virtual void receiveRunResult(const RunResult&) = 0;
};

class RunnerConnection
{
public:
  RunnerConnection(boost::asio::io_service& io_service)
    : connection_(new Connection(io_service))
  {
  }
  
  virtual ~RunnerConnection()
  {
  }
  
  void start();
  void sendMessage(Message& message);
  void sendRunOrder(boost::uint32_t run_id, boost::uint32_t problem_id, boost::uint32_t language_id, std::string attachment);  
  void sendProblemSet(boost::uint32_t problem_id, std::string attachment);  
  
  
  void addListener(RunnerConnectionListener& rcl);
  void removeListener(RunnerConnectionListener& rcl);
  
private:
  void handleConnect(const boost::system::error_code& error,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
  void handleSend(const boost::system::error_code& e, connection_ptr conn);
  void handleReceive(const boost::system::error_code& e);
    void disconnect();
  
  std::string address_;
  connection_ptr connection_;
  Message outboundMessage_;
  Message inboundMessage_;
  boost::ptr_vector<RunnerConnectionListener> listeners_;
  bool authenticated_;
  bool hasPinged_;
  std::string challenge_;
  
  std::string secret_;
  uint64_t pingTime_;
};

#endif // RUNNERCONNECTION_H
