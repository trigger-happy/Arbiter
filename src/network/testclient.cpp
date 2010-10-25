#include "testclient.h"

int main() {
  boost::asio::io_service io_service;
  RunnerNetwork rn(io_service, "localhost", 9999, 10);
  testclient ts(rn);
  rn.addListener(ts);
  io_service.run();
  std::cout << "asdsad" << std::endl;
  return 0;
}