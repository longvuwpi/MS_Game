//
// Client.cpp - a NetworkNode.
//

// Engine includes.
#include "EventNetwork.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"
#include "EventStep.h"
#include "EventCustomNetwork.h"

// Game includes.
#include "Client.h"
#include "Explosion.h"
#include "Reticle.h"
#include "Hero.h"
#include "Bullet.h"
#include "BulletTrail.h"
#include "Saucer.h"
#include "Level.h"
#include "util.h"
#include "GameStart.h"
#include "GameOver.h"
#include "ReadyButton.h"
#include "DamageIndicator.h"

// Constructor, connecting to server.
Client::Client(std::string server_name) {

	// Set as network client.
	setType("Client");
	NM.setServer(false);

	// Get player input so can send to server.
	registerInterest(df::MSE_EVENT);
	registerInterest(df::KEYBOARD_EVENT);
	registerInterest(df::STEP_EVENT);

	// Connect.
	std::string server_port = df::DRAGONFLY_PORT;
	LM.writeLog("Client::Client(): Connecting to %s at port %s.",
		server_name.c_str(), server_port.c_str());
	int sock_in = NM.connect(server_name, server_port);
	if (sock_in < 0)
		LM.writeLog("Client::Client(): Error!  Unable to connect.");
	//else socket_index = sock_in;

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
		if (mouseInWindow()) {
			//LM.writeLog("Client::eventHandler(): sending mouse input to server.");
			const df::EventMouse *p_me = dynamic_cast <const df::EventMouse *> (p_e);
			sendMessage(df::MOUSE_INPUT,
				p_me->getMouseAction(),
				p_me->getMouseButton(),
				p_me->getMousePosition());
			return 1;
		}
	}

	if (p_e->getType() == df::KEYBOARD_EVENT) {
		if (mouseInWindow()) {

			//LM.writeLog("Client::eventHandler(): sending keyboard input to server.");
			const df::EventKeyboard *p_keyboard_event = dynamic_cast <const df::EventKeyboard *> (p_e);
			sendMessage(df::KEYBOARD_INPUT, p_keyboard_event->getKeyboardAction(), p_keyboard_event->getKey());

			df::Object *hero = WM.objectWithId(hero_id);
			if (hero != NULL) {
				if ((hero->getType() == "Hero") && (((Hero *)hero)->isPredicted())) {
					hero->eventHandler(p_e);
				}
			}
			return 1;
		}
	}

	if (p_e->getType() == df::STEP_EVENT) {
		df::Object *hero = WM.objectWithId(hero_id);
		if (hero != NULL) {
			LM.writeLog("There are currently %d saucers", WM.objectsOfType("Saucer").getCount());
			LM.writeLog("Client following hero id %d", hero->getId());
			LM.writeLog("Hero is %s", (((Hero*)hero)->isPredicted()) ? "predicted" : "nonpredicted");
			WM.setViewPosition(hero->getPosition() + df::Vector(WM.getView().getHorizontal() / 3, 0));
		}
	}

	if (p_e->getType() == df::NETWORK_CUSTOM_EVENT) {
		LM.writeLog("Client::eventHandler(): received custom network event.");
		std::string message;
		memcpy(&message, ((df::EventCustomNetwork *) p_e)->getMessage(), sizeof(int));
		LM.writeLog("\tmessage: %s", message);
		hero_id = std::stoi(message);
		return 1;
	}

	// Call parent event handler.
	return NetworkNode::eventHandler(p_e);
}

// Handle incoming connect message.
int Client::handleConnect(const df::EventNetwork *p_e) {
	//socket_index = p_e->getSocketIndex();
	LM.writeLog("Client::handleConnect(): Client connected!");
	return 0;
}

// Create Object of given type.
// Return pointer to Object.
df::Object *Client::createObject(std::string object_type) {

	//LM.writeLog("Client::createObject(): creating object type '%s'",
	//	object_type.c_str());

	Object *p_obj = NULL;
	if (object_type == "Explosion")
		p_obj = new Explosion;
	else if (object_type == "ReadyButton")
		p_obj = new ReadyButton();
	else if (object_type == "GameStart")
		p_obj = new GameStart();
	else if (object_type == "Hero")
	{
		int temp = df::Object::max_id;

		df::ObjectList hero_list = WM.objectsOfType("Hero");
		if (hero_list.isEmpty()) {
			p_obj = new Hero(hero_id, 20);
		}
		else {
			p_obj = new Hero(hero_id, 40);
		}
		df::Object::max_id = temp;
	}
	else if (object_type == "Platform")
		p_obj = new Platform(df::Vector(0, 0));
	else if (object_type == "Bullet")
		p_obj = new Bullet();
	else if (object_type == "BulletTrail")
		p_obj = new BulletTrail();
	else if (object_type == "Saucer")
		p_obj = new Saucer();
	else if (object_type == "Level")
		p_obj = new Level();
	else if (object_type == "DamageIndicator")
		p_obj = new DamageIndicator();
	else if (object_type == "GameOver")
		p_obj = new GameOver();
	else
		LM.writeLog("Client::createObject(): uknown object type '%s'",
			object_type.c_str());

	return p_obj;
}

int Client::getHeroId() {
	return hero_id;
}