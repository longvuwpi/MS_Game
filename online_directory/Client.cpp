//
// Client.cpp - a NetworkNode.
//

// Engine includes.
#include "EventNetwork.h"
#include "LogManager.h"
#include "NetworkManager.h"

// Game includes.
#include "Client.h"
#include "Explosion.h"
#include "Reticle.h"
#include "Hero.h"
// Constructor, connecting to server.
Client::Client(std::string server_name) {

  // Set as network client.
  setType("Client");
  NM.setServer(false);

  // Get player input so can send to server.
  registerInterest(df::MSE_EVENT);
  registerInterest(df::KEYBOARD_EVENT);

  // Connect.
  std::string server_port = df::DRAGONFLY_PORT;
  LM.writeLog("Client::Client(): Connecting to %s at port %s.",
	      server_name.c_str(), server_port.c_str());
  if (NM.connect(server_name, server_port) < 0)
    LM.writeLog("Client::Client(): Error!  Unable to connect.");

  LM.writeLog("Client::Client(): Client started.");
}

Client::~Client() {
  // Close socket.
  if (NM.isConnected())
    NM.close();
}

// Handle event.
// Return 0 if ignored, else 1.
int Client::eventHandler(const df::Event *p_e) {

  // Send mouse event to server.
  if (p_e->getType() == df::MSE_EVENT) {
    LM.writeLog("Client::eventHandler(): sending mouse input to server.");
    const df::EventMouse *p_me = dynamic_cast <const df::EventMouse *> (p_e);
    sendMessage(df::MOUSE_INPUT,
		p_me -> getMouseAction(),
		p_me -> getMouseButton(),
		p_me -> getMousePosition());
    return 1;
  }
  
  if (p_e->getType() == df::KEYBOARD_EVENT) {
	  LM.writeLog("Client::eventHandler(): sending keyboard input to server.");
	  const df::EventKeyboard *p_keyboard_event = dynamic_cast <const df::EventKeyboard *> (p_e);
	  sendMessage(df::KEYBOARD_INPUT, p_keyboard_event->getKeyboardAction(), p_keyboard_event->getKey());
	  return 1;
  }

  // Call parent event handler.
  return NetworkNode::eventHandler(p_e);
}

// Handle incoming connect message.
int Client::handleConnect(const df::EventNetwork *p_e) {
  LM.writeLog("Client::handleConnect(): Client connected!");
  return 0;
}

// Create Object of given type.
// Return pointer to Object.
df::Object *Client::createObject(std::string object_type) {

  LM.writeLog("Client::createObject(): creating object type '%s'",
	      object_type.c_str());

  Object *p_obj = NULL;
  if (object_type == "Explosion")
	  p_obj = new Explosion;
  else if (object_type == "Hero")
	  p_obj = new Hero();
  else if (object_type == "Platform")
	  p_obj = new Platform(df::Vector(0,0));
  else
    LM.writeLog("Client::createObject(): uknown object type '%s'",
		object_type.c_str());

  return p_obj;
}
