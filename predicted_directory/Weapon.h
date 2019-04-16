#pragma once
#include "Object.h"
#include "Sprite.h"
#include "Clock.h"

#include "Hero.h"

class Hero;

enum WeaponType {SNIPER, RIFLE, LAUNCHER};

const int weapon_att_count = 6;
enum WeaponAttribute{FIRE_COUNT_DOWN, AMMO_LOADED, AMMO_BACKUP, IS_RELOADING, IS_SCOPING, CURRENT_TARGET};

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
	int bullets_fired;
	int bullets_hit;
	bool bullet_affected_by_gravity;
	bool reloading; //is the gun reloading
	bool is_scoping;
	bool weapon_modified[weapon_att_count];
	float bullet_weight;
	float bullet_radius_of_effect;
	float reload_duration;
	float inaccuracy;
	float inaccuracy_spread;
	float recoil;
	float shot_recoil;
	df::Sprite *bullet_sprite;
	df::Clock *clock;
	Hero *hero; 
	void step();
	df::Vector calculateInaccurateTarget(df::Vector target);
	df::Vector current_target;
public:
	Weapon(std::string weaponName, WeaponType weaponType, Hero *owner, int bulletSpeed, int fireRate, int ammoLoadedMax, int ammoBackupMax, int damage,bool affectedByGravity, float bulletWeight, float radiusOfEffect, float reloadDuration);
	~Weapon();
	void fire(df::Vector target);
	int eventHandler(const df::Event *p_e);
	std::string getWeaponName();
	bool getBulletAffectedByGravity();
	bool isScoping();
	bool canShoot();

	float getBulletWeight();
	float getBulletRadiusOfEffect();
	int getBulletSpeed();
	int getDamage();
	void reload();
	void refillAmmo();
	void toggleScope();
	void dealDamageAt(df::Vector target, bool drawTrail);
	int getAmmoLoaded();
	int getAmmoBackup();
	int getAmmoLoadedMax();
	int getAmmoBackupMax();
	WeaponType getWeaponType();

	bool isModified() const override;
	// Custom serialize for local attributes.
	std::string serialize(std::string all = "");
	// Custom deserialize for local attributes.
	int deserialize(std::string str);
};
