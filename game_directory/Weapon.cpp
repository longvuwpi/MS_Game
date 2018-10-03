#include "Weapon.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "EventStep.h"
#include "Bullet.h"

Weapon::Weapon(std::string weaponName, int bulletSpeed, int fireRate, bool affectedByGravity, float bulletWeight, float radiusOfEffect) {
	weapon_name = weaponName;
	bullet_speed = bulletSpeed;
	fire_rate = fireRate;
	fire_count_down = fireRate;
	bullet_weight = bulletWeight;
	affected_by_gravity = affectedByGravity;
	radius_of_effect = radiusOfEffect;
	ammo = 30;
	setType("Weapon");
	bullet_sprite = RM.getSprite(weaponName + "_bullet");
	if (!bullet_sprite)
		LM.writeLog("bullet sprite not found");

	registerInterest(df::STEP_EVENT);

}

void Weapon::fire(df::Vector origin, df::Vector target) {

	// See if time to fire.
	if (fire_count_down > 0)
		return;
	fire_count_down = fire_rate;
	ammo--;
	// Fire Bullet towards target.
	// Compute normalized vector to position, then scale by speed (1).
	df::Vector v = target - origin;
	//df::Vector v = getDirection();
	v.normalize();
	v.scale(bullet_speed);
	printf("bullet velocity %f,%f\n", v.getX(), v.getY());
	Bullet *p = new Bullet(origin, bullet_sprite, affected_by_gravity, bullet_weight, radius_of_effect);
	p->setVelocity(v);

	// Play "fire" sound.
	df::Sound *p_sound = df::ResourceManager::getInstance().getSound(weapon_name + "_fire");
	p_sound->play();
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
	// Fire countdown.
	fire_count_down--;
	if (fire_count_down < 0)
		fire_count_down = 0;
}

std::string Weapon::getWeaponName() {
	return weapon_name;
}

int Weapon::getAmmo() {
	return ammo;
}