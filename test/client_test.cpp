#include "../src/network/RunnerNetwork.h"
#include <iostream>

class ListenerImpl : public RunnerNetworkListener
{
public:
  RunnerNetwork& rn_;
  
  ListenerImpl(RunnerNetwork& rn) : rn_(rn) {
    rn.setListener(*this);
  }
  
  virtual void authenticated() {
    std::cout << "Authenticated" << std::endl;
  }
  virtual bool disconnected(const boost::system::error_code& ec) {
    std::cout << ec.message() << std::endl;
    std::cout << "Disconnected" << std::endl;
    return true;
  }
  virtual void receiveRunOrder(RunOrder ro) {
    std::cout << "Received run order" << std::endl;
    std::cout << ro.run_id() << std::endl;
    std::cout << ro.problem_id() << std::endl;
    std::cout << ro.problem_hash() << std::endl;
    std::cout << ro.language_id() << std::endl;
    std::cout << ro.language_hash() << std::endl;
    std::cout << ro.attachment() << std::endl;
    
    rn_.requestProblemSet("1");
  }
  virtual void receiveProblemSet(ProblemSet ps) {
    std::cout << "Received problem set" << std::endl;
    std::cout << ps.problem_id() << std::endl;
    std::cout << ps.problem_hash() << std::endl;
    std::cout << ps.attachment() << std::endl;
    
    rn_.requestLanguage(1);
  }
  virtual void receiveLanguage(Language l) {
    std::cout << "Received language" << std::endl;
    std::cout << l.language_id() << std::endl;
    std::cout << l.language_hash() << std::endl;
    std::cout << l.attachment() << std::endl;
    
    rn_.sendRunResult(1, 1, "stdout", "stderr", 10);
  }
};

int main() {
  RunnerNetwork rn("localhost", 9999, "asdf", 10);
  ListenerImpl ts(rn);
  rn.start();
  return 0;
}