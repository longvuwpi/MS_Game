//
// Bullet.cpp
//

// Engine includes.
#include "EventOut.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "EventStep.h"

// Game includes.
#include "Bullet.h"
#include "Saucer.h"
#include "EventNuke.h"
#include "Explosion.h"

Bullet::Bullet(df::Vector hero_pos, df::Sprite *sprite, bool affectedByGravity, float bulletWeight, float radiusOfEffect) {

	// Link to "bullet" sprite.
	setSprite(sprite);
	setSpriteSlowdown(7);

	// Make the Bullets soft so can pass through Hero.
	setSolidness(df::SOFT);

	// Set bullet properties
	affected_by_gravity = affectedByGravity;
	weight = bulletWeight;
	radius_of_effect = radiusOfEffect;

	// Set other object properties.
	setType("Bullet");

	// Set starting location, based on hero's position passed in.
	df::Vector p(hero_pos.getX(), hero_pos.getY());
	setPosition(p);

	if (affected_by_gravity) {
		setAcceleration(df::Vector(0, weight));
	}
}

// Handle event.
// Return 0 if ignored, else 1.
int Bullet::eventHandler(const df::Event *p_e) {

	if (p_e->getType() == df::OUT_EVENT) {
		out();
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

// If Bullet moves outside world, mark self for deletion.
void Bullet::out() {
	WM.markForDelete(this);
}

// If Bullet hits Saucer, mark Saucer and Bullet for deletion.
void Bullet::hit(const df::EventCollision *p_collision_event) {
	if ((p_collision_event->getObject1()->getType() == "Saucer") ||
		(p_collision_event->getObject2()->getType() == "Saucer")) {
		//WM.markForDelete(p_collision_event->getObject1());
		//WM.markForDelete(p_collision_event->getObject2());

		//if it the bullet as an area of effect, create an explosion and send an EventNuke (nearby objects will be affected)
		if (radius_of_effect > 0) {
			Explosion *p_explosion = new Explosion("nuke", radius_of_effect);
			p_explosion->setPosition(getPosition());
			EventNuke nuke(getPosition(), radius_of_effect);
			WM.onEvent(&nuke);
		}
		WM.markForDelete(this);
	}
}