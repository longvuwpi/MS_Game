//
// Bullet.h
//
#pragma once
#include "EventCollision.h"
#include "Object.h"
#include "Sprite.h"
#include "Weapon.h"

enum BulletType { HERO_BULLET, ENEMY_BULLET };

class Bullet : public df::Object {
private:
	BulletType bullet_type;
	df::Vector last_position;
	float radius_of_effect;
	int damage;
	Weapon *shot_from_weapon;
public:
	bool wasHit;
	Bullet(df::Vector start_pos, df::Sprite * sprite, Weapon * weapon);
	Bullet(df::Vector start_pos, df::Sprite * sprite, int dmg, float radius);
	int eventHandler(const df::Event *p_e);
	void step();
	void hit(const df::EventCollision *p_collision_event);
	int getDamage();
	BulletType getBulletType();
};
