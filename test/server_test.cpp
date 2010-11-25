#include "../src/network/ServerNetwork.h"
#include <iostream>

class ListenerImpl : public ServerNetworkListener, public RunnerConnectionListener
{
public:
  ServerNetwork& sn_;
  boost::shared_ptr<RunnerConnection> runner_;
  
  ListenerImpl(ServerNetwork& sn) : sn_(sn) {
    sn.setListener(*this);
  }
  
  virtual void runnerConnected(boost::shared_ptr<RunnerConnection> runner) {
    std::cout << "A runner has connected" << std::endl;
    runner_ = runner;
    runner->setListener(*this);
  }
  virtual void authenticated() {
    std::cout << "Authenticated" << std::endl;
    runner_->sendRunOrder(1, "1", "asdf", 1, "asdf", "stuff");
  }
  virtual void disconnected(const boost::system::error_code& ec) {
    std::cout << ec.message() << std::endl;
    std::cout << "A runner disconnected" << std::endl;
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
  ServerNetwork sn(9999, "asdf", 10);
  ListenerImpl ts(sn);
  sn.start();
  return 0;
}