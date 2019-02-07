//
// Client.h 
//

#ifndef CLIENT_H
#define CLIENT_H

// System includes.
#include <string>

// Game includes.
#include "NetworkNode.h"

class Client : public df::NetworkNode {

 public:

  // Constructor, connecting to server.
  Client(std::string server_name);

  // Destructor.
  ~Client();

  // Handle event.
  // Return 0 if ignored, else 1.
  int eventHandler(const df::Event *p_e);

  // Handle connect.
  int handleConnect(const df::EventNetwork *p_e);

  // Create Object of given type.
  // Return pointer to Object.
  df::Object *createObject(std::string object_type);
};

#endif
