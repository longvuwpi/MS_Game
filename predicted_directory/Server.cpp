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
#include "Saucer.h"
#include "Boss.h"
#include "Level1.h"

Server::Server(bool isPredicted) {
	is_predicted = isPredicted;
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

bool Server::getIsPredicted() {
	return is_predicted;
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

	df::ObjectList *failList = new df::ObjectList();

	// Iterate through all Objects.
	df::ObjectList ol = WM.getAllObjects();
	df::ObjectListIterator li(&ol);
	for (li.first(); !li.isDone(); li.next()) {
		Object *p_o = (Object *)li.currentObject();
		int res = 0;

		// Explosions synchronized when first created here,
		// then when deleted in Explosion destructor.
		if (p_o->isModified(df::ID) &&
			p_o->getType() == "Explosion") {
			LM.writeLog("Server::doSync(): SYNC %s (id %d).",
				p_o->getType().c_str(), p_o->getId());

			// Send to all connected sockets.
			res = sendMessage(df::SYNC_OBJECT, p_o);
		}

		// Reticle(s) synchronized when first created or when change position.
		//if (p_o -> isModified(df::POSITION) &&
		//    p_o -> getType() == "Reticle") {
		//  LM.writeLog("Server::doSync(): SYNC %s (id %d).",
			  //p_o->getType().c_str(), p_o->getId());

		//  // Send to all connected sockets.
		//  sendMessage(df::SYNC_OBJECT, p_o);
		//}

		if (p_o->getType() == "Hero") {
			if (p_o->isModified()) {
				LM.writeLog("Server::doSync(): SYNC %s (id %d).",
					p_o->getType().c_str(), p_o->getId());

				// Send to all connected sockets.
				res = sendMessage(df::SYNC_OBJECT, p_o);
			}
			else {
				LM.writeLog("Server::doSync(): Hero was not modified");
			}
		}

		if ((p_o->isModified(df::ID) ||
			(p_o->isModified(df::POSITION))) &&
			p_o->getType() == "Platform") {
			LM.writeLog("Server::doSync(): SYNC %s (id %d).",
				p_o->getType().c_str(), p_o->getId());

			// Send to all connected sockets.
			res = sendMessage(df::SYNC_OBJECT, p_o);
		}

		if ((p_o->getType() == "Weapon") && p_o->isModified()) {
			LM.writeLog("Server::doSync(): SYNC %s (id %d).",
				p_o->getType().c_str(), p_o->getId());
			res = sendMessage(df::SYNC_OBJECT, p_o);
		}

		if ((p_o->getType() == "Bullet") && p_o->isModified()) {
			LM.writeLog("Server::doSync(): SYNC %s (id %d).",
				p_o->getType().c_str(), p_o->getId());
			res = sendMessage(df::SYNC_OBJECT, p_o);
		}

		if ((p_o->getType() == "BulletTrail") && p_o->isModified(df::ID)) {
			//LM.writeLog("Server::doSync(): SYNC %s (id %d).",
			//    p_o->getType().c_str(), p_o->getId());
			//res = sendMessage(df::SYNC_OBJECT, p_o);
		}

		if ((p_o->isModified(df::ID) ||
			(p_o->isModified(df::POSITION))) &&
			p_o->getType() == "Saucer") {
			LM.writeLog("Server::doSync(): SYNC %s (id %d).",
				p_o->getType().c_str(), p_o->getId());

			// Send to all connected sockets.
			res = sendMessage(df::SYNC_OBJECT, p_o);
		}

		if (res == -1) {
			failList->insert(p_o);
		}

	} // End of iterate through all Objects.

	df::ObjectListIterator fli(failList);
	for (fli.first(); !fli.isDone(); fli.next()) {
		Object *p_fo = (Object *)fli.currentObject();

		LM.writeLog("Server::resending sync message(): SYNC %s (id %d).",
			p_fo->getType().c_str(), p_fo->getId());

		sendMessage(df::SYNC_OBJECT, p_fo, "ALL");

	}
}

// Handle accept.
int Server::handleAccept(const df::EventNetwork *p_e) {

	//// Spawn a Reticle for Client.
	////new Reticle(p_e -> getSocketIndex());
	//new Platform(df::Vector(70, 55));
	//new Platform(df::Vector(133, 60));
	//new Platform(df::Vector(150, 27));
	//new Platform(df::Vector(196, 47));
	//new Platform(df::Vector(225, 30));
	//new Platform(df::Vector(263, 65));
	//new Platform(df::Vector(317, 60));
	//new Platform(df::Vector(330, 40));
	//new Platform(df::Vector(371, 55));
	//new Hero();

	////Saucer *saucer = new Saucer(15, 5, 0, 3, 1);
	//df::Vector spawnPos(WM.getView().getCorner().getX() + WM.getView().getHorizontal()-30, 30);
	////saucer->setPosition(spawnPos);
	////saucer->markStart();

	//Boss *boss = new Boss(1100);
	//boss->setPosition(spawnPos);
	//boss->createWeakPoint(df::Vector(-8.0f, -10.0f), 5, 500);
	//boss->createWeakPoint(df::Vector(10.0f, -9.0f), 5, 500);
	
	int sock_in = p_e->getSocketIndex();
	LM.writeLog("Server::handleAccept(): server now connected socket: %d",
		sock_in);

	int hero_id = 0;

	if (sock_in == 0) {
		Hero *newHero = new Hero(0, 20, is_predicted);
		Level1 *level1 = new Level1();
		level1->start();
		hero_id = newHero->getId();
	}
	else if (sock_in == 1) {
		int temp = df::Object::max_id;
		Hero *newHero = new Hero(1, 40, is_predicted);
		df::Object::max_id = temp;
		hero_id = newHero->getId();
	}

	//std::string id_str = "Your hero id: " + std::to_string(hero_id);
	std::string id_str = std::to_string(hero_id);
	const char *buff = id_str.c_str();
	LM.writeLog("Server::eventHandler(): sending hero id message: '%s'",
		buff);

	const char *buff2 = is_predicted ? " true" : "false";
	LM.writeLog("Server::eventHandler(): sending is_predicted message: '%s'",
		buff2);
	
	// Send to newly connected socket.
	sendMessage(df::CUSTOM_MESSAGE, (int)strlen(buff) + 1, buff, sock_in);
	//sendMessage(df::CUSTOM_MESSAGE, (int)strlen(buff2) + 1, buff2, sock_in);

	// Send all Reticles and Explosions to newly connected client.
	df::ObjectList ol = WM.getAllObjects();
	df::ObjectListIterator li(&ol);
	for (li.first(); !li.isDone(); li.next()) {
		Object *p_o = (Object *)li.currentObject();
		if (p_o->getType() == "Explosion" || p_o->getType() == "Hero" || p_o->getType() == "Platform" || p_o->getType() == "Saucer" || p_o->getType() == "Level")
			sendMessage(df::SYNC_OBJECT, p_o, "ALL");
			//sendMessage(df::SYNC_OBJECT, p_o, p_e->getSocketIndex());
	}

	// All is well.
	return 1;
}
