//
// Bullet.cpp
//

// Engine includes.
#include "EventOut.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "EventStep.h"
#include "DisplayManager.h"
#include "WorldManager.h"
#include "NetworkManager.h"

// Game includes.
#include "Bullet.h"
//#include "Saucer.h"
//#include "WeakPoint.h"
//#include "Boss.h"
#include "EventNuke.h"
#include "Explosion.h"
#include "BulletTrail.h"
//#include "DamageIndicator.h"
#include "Server.h"

Bullet::Bullet() {
	setType("Bullet");
	last_position = df::Vector(0, 0);
}

Bullet::Bullet(df::Vector bullet_pos, df::Sprite *sprite, Weapon *weapon) {
	// Link to "bullet" sprite.
	setSprite(sprite);
	setSpriteSlowdown(7);

	// Make the Bullets soft so can pass through Hero.
	setSolidness(df::SOFT);

	// Set bullet properties
	wasHit = false;
	damage = weapon->getDamage();
	radius_of_effect = weapon->getBulletRadiusOfEffect();
	shot_from_weapon = weapon;

	// Set other object properties.
	setType("Bullet");

	// Set starting location, based on hero's position passed in.
	df::Vector p(bullet_pos.getX(), bullet_pos.getY());
	setPosition(p);

	registerInterest(df::STEP_EVENT);

	last_position = bullet_pos;

	if (weapon->getBulletAffectedByGravity() && NM.isServer()) {
		setAcceleration(df::Vector(0, weapon->getBulletWeight()));
	}

	bullet_type = BulletType::HERO_BULLET;
}

Bullet::Bullet(df::Vector bullet_pos, df::Sprite * sprite, int dmg, float radius)
{
	// Link to "bullet" sprite.
	setSprite(sprite);
	setSpriteSlowdown(7);

	// Make the Bullets soft so can pass through Hero.
	setSolidness(df::SOFT);

	// Set bullet properties
	wasHit = false;
	damage = dmg;
	radius_of_effect = radius;
	shot_from_weapon = NULL;
	// Set other object properties.
	setType("Bullet");

	// Set starting location, based on hero's position passed in.
	df::Vector p(bullet_pos.getX(), bullet_pos.getY());
	setPosition(p);

	registerInterest(df::STEP_EVENT);

	last_position = bullet_pos;

	bullet_type = BulletType::ENEMY_BULLET;

}

Bullet::~Bullet() {
	if (NM.isServer()) {
		df::ObjectList ol = WM.objectsOfType("Server");
		df::ObjectListIterator oli(&ol);
		oli.first();
		Server *server = (Server *)oli.currentObject();
		server->sendMessage(df::DELETE_OBJECT, this);
	}
	else {
		if (getSprite() != NULL) {
			if (getSprite()->getLabel() == "GrenadeLauncher_bullet") {
				df::addParticles(50, 10, getPosition(), 0.1f, df::Vector(0, 0), 0.0f, 5.0f, 5.0f, 2.5f, 1.0f, 10, 10, (unsigned char)255, (char)255, (unsigned char)255, (unsigned char)255, (unsigned char)0, (unsigned char)100, df::ParticleClass::FIREWORK);
				//Explosion *p_explosion = new Explosion("nuke", radius_of_effect);
				//p_explosion->setPosition(getPosition());
			}
		}
	}
	// Mark Reticle for deletion.
	//WM.markForDelete(p_reticle);
}

// Handle event.
// Return 0 if ignored, else 1.
int Bullet::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::STEP_EVENT) {
		step();
		return 1;
	}
	if (NM.isServer()) {

		if (p_e->getType() == df::COLLISION_EVENT) {
			const df::EventCollision *p_collision_event = dynamic_cast <const df::EventCollision *> (p_e);
			hit(p_collision_event);
			return 1;
		}
	}

	// If get here, have ignored this event.
	return 0;
}

// If Bullet hits Saucer, mark Saucer and Bullet for deletion.
void Bullet::hit(const df::EventCollision *p_collision_event) {
	std::string type1, type2;
	type1 = p_collision_event->getObject1()->getType();
	type2 = p_collision_event->getObject2()->getType();

	if ((((type1 == "Saucer") || (type2 == "Saucer")) && (bullet_type == BulletType::HERO_BULLET)) ||
		(((type1 == "Hero") || (type2 == "Hero")) && (bullet_type == BulletType::ENEMY_BULLET))) {
		if (!wasHit) {
			//if it the bullet as an area of effect, create an explosion and send an EventNuke (nearby objects will be affected)
			if (radius_of_effect > 0) {
				//df::addParticles(50, 10, getPosition(), 0.1f, df::Vector(0, 0), 0.0f, 5.0f, 5.0f, 2.5f, 1.0f, 10, 10, (unsigned char)255, (char)255, (unsigned char)255, (unsigned char)255, (unsigned char)0, (unsigned char)100, df::ParticleClass::FIREWORK);
				//Explosion *p_explosion = new Explosion("nuke", radius_of_effect);
				//p_explosion->setPosition(getPosition());
				EventNuke nuke(getPosition(), radius_of_effect, getDamage());
				WM.onEvent(&nuke);
			}
			wasHit = true;
			//Destroy the bullet
			WM.markForDelete(this);

			//Deal damage to the entity
			if (type1 == "Hero") {
				//LM.writeLog("Hit hero");

				dynamic_cast <Hero *> (p_collision_event->getObject1())->takeDamage(p_collision_event->getPosition(), damage);
			}
			else if (type2 == "Hero") {
				//LM.writeLog("Hit hero");

				dynamic_cast <Hero *> (p_collision_event->getObject2())->takeDamage(p_collision_event->getPosition(), damage);
			}
			else {
				//LM.writeLog("Hit saucer");
				shot_from_weapon->dealDamageAt(p_collision_event->getPosition(), false);
			}
		}
	}
}

void Bullet::step() {
	df::Vector current_pos = getPosition();
	//df::Vector window_corner = WM.getView().getCorner();
	//if ((current_pos.getX() < window_corner.getX()) || (current_pos.getX() > (window_corner.getX() + WM.getView().getHorizontal())) ||
	//	(current_pos.getY() < (window_corner.getY() - 30)) || (current_pos.getY() > (window_corner.getY() + WM.getView().getVertical()))) {
	//	WM.markForDelete(this);
	//}

	df::Vector window_corner = WM.getView().getCorner();
	if ((current_pos.getX() < 0) || (current_pos.getX() > WM.getBoundary().getHorizontal()) ||
		(current_pos.getY() < (-30)) || (current_pos.getY() > WM.getBoundary().getVertical())) {
		if (NM.isServer()) {
			WM.markForDelete(this);
		}
	}

	else if (current_pos != last_position) {
			df::Vector traveled = current_pos - last_position;
			int distance = traveled.getMagnitude();
			traveled.normalize();
			df::Vector particle_dir = last_position - current_pos;
			particle_dir.normalize();

			//if (bullet_type == HERO_BULLET) {
			//	if (shot_from_weapon->getWeaponType() == WeaponType::LAUNCHER) {
			if (getSprite() != NULL) {
				if (getSprite()->getLabel() == "GrenadeLauncher_bullet") {
					df::addParticles(5, 2, current_pos + df::Vector(0, 0.5f), 2.0f, particle_dir, 0.8f, 2.0f, 1.0f, 1.0f, 1.0f, 15, 2, (unsigned char)255, (char)255, (unsigned char)255, (unsigned char)100, (unsigned char)0, (unsigned char)255, df::ParticleClass::FIREWORK);
				}
			}
			//}

			for (int i = 0; i <= distance; i++) {
				BulletTrail *trail = new BulletTrail(this);
				df::Vector scaled(traveled.getX(), traveled.getY());
				scaled.scale(i);
				trail->setPosition(last_position + scaled - df::Vector(0, 0.3f));
				trail->setVisible(isVisible());
			}
		last_position = current_pos;
	}
}

int Bullet::getDamage() {
	return damage;
}

BulletType Bullet::getBulletType() {
	return bullet_type;
}

int Bullet::deserialize(std::string str) {
	int res = df::Object::deserialize(str);
	if ((last_position.getX() == 0) && (last_position.getY() == 0)) {
		last_position = getPosition();
	}
	return res;
}