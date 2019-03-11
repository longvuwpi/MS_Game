//
// Server.h
//

#ifndef SERVER_H
#define SERVER_H

// Engine includes.
#include "NetworkNode.h"

const int MAX_CONNECTIONS=3;  // Maximum number of Clients.

class Server : public df::NetworkNode {

 private:
  // If any Objects need to be synchronized, send to client.
  void doSync();

 public:
  Server();

  // Handle event.
  // Return 0 if ignored, else 1.
  int eventHandler(const df::Event *p_e);

  // Handle accept.
  int handleAccept(const df::EventNetwork *p_e);
};

#endif
