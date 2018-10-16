#include "Weapon.h"
#include "Bullet.h"

#include "Particle.h"
#include "utility.h"
#include "EventStep.h"
#include "ResourceManager.h"
#include "LogManager.h"

Weapon::Weapon(std::string weaponName, Hero* owner, int bulletSpeed, int fireRate, int ammoLoadedMax, int ammoBackupMax, int dmg, bool affectedByGravity, float bulletWeight, float radiusOfEffect, float reloadDuration) {
	weapon_name = weaponName;
	hero = owner;
	bullet_speed = bulletSpeed;
	fire_rate = fireRate;
	fire_count_down = fireRate;
	ammo_loaded_max = ammoLoadedMax;
	ammo_loaded = ammoLoadedMax;
	ammo_backup_max = ammoBackupMax;
	ammo_backup = ammoBackupMax;
	damage = dmg;
	bullet_weight = bulletWeight;
	bullet_affected_by_gravity = affectedByGravity;
	bullet_radius_of_effect = radiusOfEffect;
	reload_duration = reloadDuration;
	clock = new df::Clock;
	reloading = false;
	setType("Weapon");
	
	df::Sprite *p_temp_sprite = RM.getSprite("AK47");
	if (!p_temp_sprite)
		LM.writeLog("Weapon: Warning! Sprite '%s' not found", "AK47");
	else {
		setSprite(p_temp_sprite);
		setTransparency('#');
	}

	setSolidness(df::SOFT);

	bullet_sprite = RM.getSprite(weaponName + "_bullet");
	if (!bullet_sprite)
		LM.writeLog("bullet sprite not found");

	registerInterest(df::STEP_EVENT);

}

void Weapon::fire(df::Vector target) {
	df::Vector origin = getPosition() + (df::Vector(getBox().getHorizontal() / 2, -1.5f));
	// See if time to fire.
	if ((fire_count_down > 0) || reloading) {
		return;
	}
	if (ammo_loaded == 0) {
		//TODO: play empty ammo sound
		return;
	}
	fire_count_down = fire_rate;
	ammo_loaded--;
	// Fire Bullet towards target.
	// Compute normalized vector to position, then scale by speed (1).
	df::Vector v = target - origin;
	//df::Vector v = getDirection();
	v.normalize();
	v.scale(bullet_speed);
	printf("bullet velocity %f,%f\n", v.getX(), v.getY());
	Bullet *p = new Bullet(origin, bullet_sprite, this);
	p->setVelocity(v);

	// Play "fire" sound.
	df::Sound *p_sound = df::ResourceManager::getInstance().getSound(weapon_name + "_fire");
	p_sound->play();
	hero->getReticle()->expand();
	//df::addParticles(30, 10, origin, 0.1f, df::Vector(0,0), 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 15, 10, (unsigned char)255, (char)255, (unsigned char)250, (unsigned char)250, (unsigned char)200, (unsigned char)100);
}

int Weapon::eventHandler(const df::Event *p_e){
	if (p_e->getType() == df::STEP_EVENT) {
		step();
		return 1;
	}

	// If get here, have ignored this event.
	return 0;
}

void Weapon::step() {
	if (reloading) {
		float time = (float)clock->split()/(float)1000000;
		//if trying to reload and the reload time has passed
		if (time >= reload_duration) {
			//then fill up ammo from the backup ammo
			int neededAmmo = ammo_loaded_max - ammo_loaded;
			if (neededAmmo > ammo_backup) {
				neededAmmo = ammo_backup;
			}
			ammo_loaded += neededAmmo;
			ammo_backup -= neededAmmo;
			reloading = false;
		}
	}

	// Fire countdown.
	fire_count_down--;
	if (fire_count_down < 0)
		fire_count_down = 0;

	setPosition(hero->getPosition() + df::Vector(3,1));
}

std::string Weapon::getWeaponName() {
	return weapon_name;
}

bool Weapon::getBulletAffectedByGravity() {
	return bullet_affected_by_gravity;
}

float Weapon::getBulletWeight() {
	return bullet_weight;
}

float Weapon::getBulletRadiusOfEffect() {
	return bullet_radius_of_effect;
}

int Weapon::getDamage() {
	return damage;
}

void Weapon::reload() {
	//if already trying to reload or full ammo or no more backup ammo
	//then can't reload anymore
	if (reloading || (ammo_loaded == ammo_loaded_max) || (ammo_backup == 0)) {
		return;
	}
	reloading = true;
	clock->delta();
}

int Weapon::getAmmoLoaded() {
	return ammo_loaded;
}

int Weapon::getAmmoBackup() {
	return ammo_backup;
}