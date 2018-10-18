#pragma once
#include "Object.h"
#include "Sprite.h"
#include "Clock.h"

#include "Hero.h"

class Hero;

enum WeaponType {SNIPER, RIFLE, LAUNCHER};

class Weapon : public df::Object {

private:
	std::string weapon_name;
	WeaponType weapon_type;
	int bullet_speed; //distance traveled in 1 frame
	int fire_rate; //1 shot every ? frames
	int fire_count_down;
	int ammo_loaded_max;
	int ammo_backup_max;
	int ammo_loaded;
	int ammo_backup;
	int damage;
	int last_shot_frame; //the frame number that the last shot was fired
	bool bullet_affected_by_gravity;
	bool reloading; //is the gun reloading
	float bullet_weight;
	float bullet_radius_of_effect;
	float reload_duration;
	float inaccuracy;
	float inaccuracy_spread;
	df::Sprite *bullet_sprite;
	df::Clock *clock;
	Hero *hero;
	void step();
	df::Vector calculateInaccurateTarget(df::Vector target);
public:
	Weapon(std::string weaponName, WeaponType weaponType, Hero *owner, int bulletSpeed, int fireRate, int ammoLoadedMax, int ammoBackupMax, int damage,bool affectedByGravity, float bulletWeight, float radiusOfEffect, float reloadDuration);
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
	WeaponType getWeaponType();
};
