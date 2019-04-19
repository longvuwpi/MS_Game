#pragma once
#include "EventCollision.h"
#include "Object.h"
#include "Sprite.h"
#include "Bullet.h"

class BulletTrail : public df::Object {

private:
	void hit(const df::EventCollision *p_collision_event);
	Bullet *owner;
	int life_time;
	int owner_id;
public:
	BulletTrail();
	~BulletTrail();
	BulletTrail(Bullet *spawner);
	int eventHandler(const df::Event *p_e);
	int getDamage();
	BulletType getBulletType();
};
