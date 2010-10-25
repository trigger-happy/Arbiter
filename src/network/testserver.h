#ifndef TESTSERVER_H
#define TESTSERVER_H

#include "ServerNetwork.h"
#include <iostream>

class testserver : public ServerNetworkListener, public RunnerConnectionListener
{
public:
  ServerNetwork& sn_;
  boost::shared_ptr<RunnerConnection> runner_;
  
  testserver(ServerNetwork& sn) : sn_(sn) {}
  
  virtual void runnerConnected(boost::shared_ptr<RunnerConnection> runner) {
    runner_ = runner;
    runner->addListener(*this);
  }
  virtual void authenticated() {
    std::cout << "Authenticated" << std::endl;
    runner_->sendRunOrder(1, "1", "asdf", 1, "asdf", "stuff");
  }
  virtual void disconnected() {
    std::cout << "Something disconnected" << std::endl;
  }  
  virtual void receiveProblemSetRequest(std::string pid) {
    std::cout << "Received problem set request " << pid << std::endl;
    runner_->sendProblemSet("1", "asdf", "problemset");
  }
  virtual void receiveLanguageRequest(boost::uint32_t lid) {
    std::cout << "Received language request " << lid << std::endl;
    runner_->sendLanguage(1, "asdf", "language");
  }
  virtual void receiveRunResult(RunResult rr) {
    std::cout << "Received run result" << std::endl;
    std::cout << rr.run_id() << std::endl;
    std::cout << rr.result() << std::endl;
    std::cout << rr.team_stdout() << std::endl;
    std::cout << rr.team_stderr() << std::endl;
    std::cout << rr.run_time() << std::endl;
  }
};

#endif // TESTSERVER_H
