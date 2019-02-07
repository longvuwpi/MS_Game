//
// Server.cpp - a NetworkNode.
//

// Engine includes.
#include "EventStep.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"

// Game includes.
#include "Reticle.h"
#include "Server.h"
#include "Platform.h"
#include "Hero.h"
#include "Weapon.h"

Server::Server() {

  // Set as network server.
  setType("Server");
  NM.setServer(true);
  LM.writeLog("Server::Server(): Setup for %d connections.",
	      MAX_CONNECTIONS);
  NM.setMaxConnections(MAX_CONNECTIONS);
  
  // Register for step events to synchronize objects.
  registerInterest(df::STEP_EVENT);

  LM.writeLog("Server::Server(): Server started");
}

// Handle event.
// Return 0 if ignored, else 1.
int Server::eventHandler(const df::Event *p_e) {

  // Step event.
  if (p_e->getType() == df::STEP_EVENT && NM.isConnected()) {
    doSync();
    return 1;
  }

  // Call parent event handler.
  return NetworkNode::eventHandler(p_e);

} // End of eventHandler().

// If any Objects need to be synchronized, send to Clients.
void Server::doSync() {

  // Iterate through all Objects.
  df::ObjectList ol = WM.getAllObjects();
  df::ObjectListIterator li(&ol);
  for (li.first(); !li.isDone(); li.next()) {
    Object *p_o = (Object *) li.currentObject();

    // Explosions synchronized when first created here,
    // then when deleted in Explosion destructor.
    if (p_o -> isModified(df::ID) &&
		p_o->getType() == "Explosion") {
      LM.writeLog("Server::doSync(): SYNC %s (id %d).",
		  p_o->getType().c_str(), p_o->getId());

      // Send to all connected sockets.
      sendMessage(df::SYNC_OBJECT, p_o);
    }

    // Reticle(s) synchronized when first created or when change position.
    if (p_o -> isModified(df::POSITION) &&
        p_o -> getType() == "Reticle") {
      LM.writeLog("Server::doSync(): SYNC %s (id %d).",
		  p_o->getType().c_str(), p_o->getId());

      // Send to all connected sockets.
      sendMessage(df::SYNC_OBJECT, p_o);
    }

	if (p_o->isModified() &&
		(p_o->getType() == "Hero")) {
		LM.writeLog("Server::doSync(): SYNC %s (id %d).",
			p_o->getType().c_str(), p_o->getId());

		// Send to all connected sockets.
		sendMessage(df::SYNC_OBJECT, p_o);
	}

  if (p_o->isModified(df::ID) &&
	  p_o->getType() == "Platform") {
	  LM.writeLog("Server::doSync(): SYNC %s (id %d).",
		  p_o->getType().c_str(), p_o->getId());

	  // Send to all connected sockets.
	  sendMessage(df::SYNC_OBJECT, p_o);
  }

  if ((p_o->getType() == "Weapon") && p_o->isModified()) {
		  LM.writeLog("Server::doSync(): SYNC %s (id %d).",
			  p_o->getType().c_str(), p_o->getId());
		  sendMessage(df::SYNC_OBJECT, p_o);
  }


} // End of iterate through all Objects.
}

// Handle accept.
int Server::handleAccept(const df::EventNetwork *p_e) {

  LM.writeLog("Server::handleAccept(): server now connected socket: %d",
	      p_e -> getSocketIndex());

  // Spawn a Reticle for Client.
  //new Reticle(p_e -> getSocketIndex());
  new Platform(df::Vector(70, 55));
  new Platform(df::Vector(133, 60));
  new Platform(df::Vector(150, 27));
  new Platform(df::Vector(196, 47));
  new Platform(df::Vector(225, 30));
  new Platform(df::Vector(263, 65));
  new Platform(df::Vector(317, 60));
  new Platform(df::Vector(330, 40));
  new Platform(df::Vector(371, 55));
  new Hero();

  // Send all Reticles and Explosions to newly connected client.
  df::ObjectList ol = WM.getAllObjects();
  df::ObjectListIterator li(&ol);
  for (li.first(); !li.isDone(); li.next()) {
    Object *p_o = (Object *) li.currentObject();
    if (p_o->getType() == "Explosion" || p_o->getType() == "Hero" || p_o->getType() == "Platform")
	  sendMessage(df::SYNC_OBJECT, p_o, p_e -> getSocketIndex());
  }
   
  // All is well.
  return 1;
}
