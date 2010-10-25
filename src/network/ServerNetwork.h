#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include <string>
#include "RunnerConnection.h"

class ServerNetwork
{
public:
  ServerNetwork()
  {
  }
  
  virtual ~ServerNetwork()
  {
  }
};

class ServerNetworkListener
{
public:
  virtual void runnerConnected(RunnerConnection*) = 0;
};

#endif // SERVER_NETWORK_H
