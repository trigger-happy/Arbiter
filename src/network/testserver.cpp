#include "testserver.h"

int main() {
  boost::asio::io_service io_service;
  ServerNetwork sn(io_service, 9999, "asdf", 10);
  testserver ts(sn);
  sn.addListener(ts);
  io_service.run();
  return 0;
}