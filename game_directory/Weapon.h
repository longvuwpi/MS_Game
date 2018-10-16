#pragma once
#include "Object.h"
#include "Sprite.h"
#include "Clock.h"

#include "Hero.h"

class Hero;

class Weapon : public df::Object {

private:
	std::string weapon_name;
	int bullet_speed;
	int fire_rate;
	int fire_count_down;
	int ammo_loaded_max;
	int ammo_backup_max;
	int ammo_loaded;
	int ammo_backup;
	int damage;
	bool bullet_affected_by_gravity;
	bool reloading;
	float bullet_weight;
	float bullet_radius_of_effect;
	float reload_duration;
	df::Sprite *bullet_sprite;
	df::Clock *clock;
	Hero *hero;
	void step();
public:
	Weapon(std::string weaponName, Hero *owner, int bulletSpeed, int fireRate, int ammoLoadedMax, int ammoBackupMax, int damage,bool affectedByGravity, float bulletWeight, float radiusOfEffect, float reloadDuration);
	void fire(df::Vector target);
	int eventHandler(const df::Event *p_e);
	std::string getWeaponName();
	bool getBulletAffectedByGravity();
	float getBulletWeight();
	float getBulletRadiusOfEffect();
	int getDamage();
	void reload();
	void refillAmmo();
	int getAmmoLoaded();
	int getAmmoBackup();
};
