//
// EventNuke.cpp
//

#include "Platform.h"

#include "LogManager.h"
#include "ResourceManager.h"
#include "EventCollision.h"
#include "NetworkManager.h"
#include "WorldManager.h"
#include "DisplayManager.h"

#include "Server.h"

Platform::Platform(df::Vector pos) {
	df::Sprite *p_temp_platform_sprite = RM.getSprite("platform");
	if (!p_temp_platform_sprite)
		LM.writeLog("Platform sprite not found");
	else {
		setSprite(p_temp_platform_sprite);
	}
	setCentered(true);
	// Make the platforms hard.
	setSolidness(df::SOFT);
	// Set other object properties.
	setType("Platform");

	setPosition(pos);
}

Platform::~Platform() {
	if (NM.isServer()) {
		df::ObjectList ol = WM.objectsOfType("Server");
		df::ObjectListIterator oli(&ol);
		oli.first();
		Server *server = (Server *)oli.currentObject();
		server->sendMessage(df::DELETE_OBJECT, this);
	}
}

int Platform::eventHandler(const df::Event *p_e) {

	if ((p_e->getType() == df::COLLISION_EVENT) && (!NM.isServer())) {
		const df::EventCollision *p_collision_event = dynamic_cast <const df::EventCollision *> (p_e);

		bool hero_landed = false;
		Hero *hero;
		std::string type1, type2;
		type1 = p_collision_event->getObject1()->getType();
		type2 = p_collision_event->getObject2()->getType();

		if (type1 == "Hero") {
			hero = (Hero *)(p_collision_event->getObject1());
			hero_landed = true;
		}
		else if (type2 == "Hero") {
			hero = (Hero *)(p_collision_event->getObject2());
			hero_landed = true;
		}

		if (hero_landed) {
			//DM.drawString(getPosition() - df::Vector(0, 2.5f), "*******Collided with hero*********", df::CENTER_JUSTIFIED, df::WHITE);
			//LM.writeLog("Platform id %d :: collided with hero", getId());
			if (hero->isMainHero() && hero->isPredicted()) {
				hero->landedOn(this);
			}
		}

		return 1;
	}
	
	//if (p_e->getType() == PLAYER_JUMPING_EVENT) {
	//	setSolidness(df::SOFT);
	//}

	//if (p_e->getType() == PLAYER_FALLING_EVENT) {
	//	const EventPlayerFalling *p_event = dynamic_cast <EventPlayerFalling const *> (p_e);
	//	const df::Vector playerPos = p_event->getPlayerPos();
	//	df::Vector highest = getPosition() - *(new df::Vector(0, 12));
	//	if (playerPos.getY() < highest.getY()) {
	//		setSolidness(df::HARD);
	//	}
	//}

	return 0;
}
