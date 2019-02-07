//
// EventNuke.cpp
//

#include "Platform.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "EventCollision.h"

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

int Platform::eventHandler(const df::Event *p_e) {

	if (p_e->getType() == df::COLLISION_EVENT) {
		//printf("collided with hero");
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

