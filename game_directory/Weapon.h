#pragma once
#include "Object.h"
#include "Sprite.h"
#include "Hero.h"

class Weapon : public df::Object {

private:
	std::string weapon_name;
	int bullet_speed;
	int fire_rate;
	int fire_count_down;
	int ammo;
	bool bullet_affected_by_gravity;
	float bullet_weight;
	float bullet_radius_of_effect;
	df::Sprite *bullet_sprite;
	Hero *hero;
	void step();
public:
	Weapon(std::string weaponName, Hero *owner, int bulletSpeed, int fireRate,bool affectedByGravity, float bulletWeight, float radiusOfEffect);
	void fire(df::Vector origin, df::Vector target);
	int eventHandler(const df::Event *p_e);
	std::string getWeaponName();
	int getAmmo();
	bool getBulletAffectedByGravity();
	float getBulletWeight();
	float getBulletRadiusOfEffect();
};
