#pragma once
#include "Object.h"
#include "Sprite.h"

class Weapon : public df::Object {

private:
	std::string weapon_name;
	int bullet_speed;
	int fire_rate;
	int fire_count_down;
	int ammo;
	bool affected_by_gravity;
	float bullet_weight;
	float radius_of_effect;
	df::Sprite *bullet_sprite;

	void step();
public:
	Weapon(std::string weaponName, int bulletSpeed, int fireRate,bool affectedByGravity, float bulletWeight, float radiusOfEffect);
	void fire(df::Vector origin, df::Vector target);
	int eventHandler(const df::Event *p_e);
	std::string getWeaponName();
	int getAmmo();
};
