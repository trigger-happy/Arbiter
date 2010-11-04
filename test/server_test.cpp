#include "../src/network/ServerNetwork.h"
#include <iostream>

class ListenerImpl : public ServerNetworkListener, public RunnerConnectionListener
{
public:
  ServerNetwork& sn_;
  boost::shared_ptr<RunnerConnection> runner_;
  
  ListenerImpl(ServerNetwork& sn) : sn_(sn) {
    sn.addListener(*this);
  }
  
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

int main() {
  boost::asio::io_service io_service;
  ServerNetwork sn(io_service, 9999, "asdf", 10);
  ListenerImpl ts(sn);
  io_service.run();
  return 0;
}