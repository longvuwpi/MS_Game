#include "BulletTrail.h"

#include "ResourceManager.h"
#include "LogManager.h"
#include "EventStep.h"
#include "EventOut.h"
#include "WorldManager.h"
#include "NetworkManager.h"
#include "Server.h"

BulletTrail::BulletTrail() {
	df::Sprite *p_temp_sprite = RM.getSprite("bullet_trail");
	if (!p_temp_sprite)
		LM.writeLog("Explosion::Explosion(): Warning! Sprite '%s' not found", "bullet_trail");
	else
		setSprite(p_temp_sprite);

	setType("BulletTrail");
	setSolidness(df::SOFT);
	setTransparency('#');    // Transparent character.

	life_time = 1;
}

BulletTrail::BulletTrail(Bullet *spawner) {
	df::Sprite *p_temp_sprite = RM.getSprite("bullet_trail");
	if (!p_temp_sprite)
		LM.writeLog("Explosion::Explosion(): Warning! Sprite '%s' not found", "bullet_trail");
	else
		setSprite(p_temp_sprite);

	setType("BulletTrail");
	setSolidness(df::SOFT);
	setTransparency('#');    // Transparent character.

	owner = spawner;

	life_time = 1;

	registerInterest(df::STEP_EVENT);
}

BulletTrail::~BulletTrail() {
	if (NM.isServer()) {
		df::ObjectList ol = WM.objectsOfType("Server");
		df::ObjectListIterator oli(&ol);
		oli.first();
		Server *server = (Server *)oli.currentObject();
		server->sendMessage(df::DELETE_OBJECT, this);
	}
}

int BulletTrail::eventHandler(const df::Event *p_e) {
	if (NM.isServer()) {
		if (p_e->getType() == df::STEP_EVENT) {
			life_time--;
			if (life_time <= 0) {
				WM.markForDelete(this);
			}
			return 1;
		}

		if (p_e->getType() == df::OUT_EVENT) {
			WM.markForDelete(this);
			return 1;
		}

		if (p_e->getType() == df::COLLISION_EVENT) {
			const df::EventCollision *p_collision_event = dynamic_cast <const df::EventCollision *> (p_e);
			hit(p_collision_event);
			return 1;
		}
	}
	// If get here, have ignored this event.
	return 0;

}

void BulletTrail::hit(const df::EventCollision *p_collision_event) {
	if (!owner->wasHit) owner->hit(p_collision_event);
}

int BulletTrail::getDamage() {
	return owner->getDamage();
}

BulletType BulletTrail::getBulletType() {
	return owner->getBulletType();
}