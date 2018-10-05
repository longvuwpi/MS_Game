#include "Weapon.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "EventStep.h"
#include "Bullet.h"

Weapon::Weapon(std::string weaponName, Hero* owner, int bulletSpeed, int fireRate, bool affectedByGravity, float bulletWeight, float radiusOfEffect) {
	weapon_name = weaponName;
	hero = owner;
	bullet_speed = bulletSpeed;
	fire_rate = fireRate;
	fire_count_down = fireRate;
	bullet_weight = bulletWeight;
	bullet_affected_by_gravity = affectedByGravity;
	bullet_radius_of_effect = radiusOfEffect;
	ammo = 30;
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
	Bullet *p = new Bullet(origin, bullet_sprite, this);
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

	setPosition(hero->getPosition() + df::Vector(3,1));
}

std::string Weapon::getWeaponName() {
	return weapon_name;
}

int Weapon::getAmmo() {
	return ammo;
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