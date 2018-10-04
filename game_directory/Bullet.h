//
// Bullet.h
//
#pragma once
#include "EventCollision.h"
#include "Object.h"
#include "Sprite.h"
#include "Weapon.h"

class Bullet : public df::Object {

private:
	void out();
	df::Vector last_position;
	Weapon *came_from_weapon;
public:
	bool wasHit;
	Bullet(df::Vector hero_pos, df::Sprite * sprite, Weapon * weapon);
	int eventHandler(const df::Event *p_e);
	void step();
	void hit(const df::EventCollision *p_collision_event);

};
