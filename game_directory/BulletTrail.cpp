#include "BulletTrail.h"

#include "ResourceManager.h"
#include "LogManager.h"
#include "EventStep.h"
#include "EventOut.h"
#include "WorldManager.h"

BulletTrail::BulletTrail(Bullet *spawner) {
	df::Sprite *p_temp_sprite = RM.getSprite("bullet_trail");
	if (!p_temp_sprite)
		LM.writeLog("Explosion::Explosion(): Warning! Sprite '%s' not found", "bullet_trail");
	else
		setSprite(p_temp_sprite);

	setType("BulletTrail");

	setTransparency('#');    // Transparent character.

	owner = spawner;

	life_time = 1;

	registerInterest(df::STEP_EVENT);
}

int BulletTrail::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::STEP_EVENT) {
		if (life_time <= 0) {
			WM.markForDelete(this);
		}
		life_time--;
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

	// If get here, have ignored this event.
	return 0;

}

void BulletTrail::hit(const df::EventCollision *p_collision_event) {
	if ((p_collision_event->getObject1()->getType() == "Hero") ||
		(p_collision_event->getObject2()->getType() == "Hero")) {
		return;
	}
		if (!owner->wasHit) owner->hit(p_collision_event);
}

int BulletTrail::getDamage() {
	return owner->getDamage();
}

BulletType BulletTrail::getBulletType() {
	return owner->getBulletType();
}