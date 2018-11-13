#include "Weapon.h"
#include "Bullet.h"
#include "Saucer.h"
#include "WeakPoint.h"
#include "Boss.h"
#include "Explosion.h"

#include "WorldManager.h"
#include "Particle.h"
#include "utility.h"
#include "EventStep.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "GameManager.h"
#include "EventMouse.h"
#include "DisplayManager.h"

Weapon::Weapon(std::string weaponName, WeaponType weaponType, Hero* owner, int bulletSpeed, int fireRate, int ammoLoadedMax, int ammoBackupMax, int dmg, bool affectedByGravity, float bulletWeight, float radiusOfEffect, float reloadDuration) {
	weapon_name = weaponName;
	weapon_type = weaponType;
	hero = owner;
	bullet_speed = bulletSpeed;
	fire_rate = fireRate;
	fire_count_down = 0;
    shootingcount = 3;
	ammo_loaded_max = ammoLoadedMax;
	ammo_backup_max = ammoBackupMax;
	refillAmmo();
    pickPower();
	damage = dmg;
    newDamage = damage + (0.15 * damage);
	bullet_weight = bulletWeight;
	bullet_affected_by_gravity = affectedByGravity;
	bullet_radius_of_effect = radiusOfEffect;
	reload_duration = reloadDuration;
	recoil = 0;
	shot_recoil = 2.0f;
	clock = new df::Clock;
	reloading = false;
	is_scoping = false;
	setType("Weapon");

	df::Sprite *p_temp_sprite = RM.getSprite(weapon_name);
	if (!p_temp_sprite) {
		LM.writeLog("Weapon: Warning! Sprite '%s' not found", weapon_name.c_str());
		p_temp_sprite = RM.getSprite("AK47");
	}

	setSprite(p_temp_sprite);
	setTransparency('#');

	setSolidness(df::SOFT);

	bullet_sprite = RM.getSprite(weaponName + "_bullet");
	if (!bullet_sprite)
		LM.writeLog("bullet sprite not found");

	registerInterest(df::STEP_EVENT);

	switch (weapon_type) {
	case WeaponType::RIFLE:
		inaccuracy = 0;
		inaccuracy_spread = 0;
		break;
	case WeaponType::SNIPER:
		inaccuracy = 0;
		inaccuracy_spread = 4.5f;
		break;
	default:
		inaccuracy = 0;
		inaccuracy_spread = 0;
		break;
	}
}

df::Vector Weapon::calculateInaccurateTarget(df::Vector target) {
	float inaccuracy_random_spread_y = -inaccuracy_spread + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * inaccuracy_spread)));
	if (weapon_type == WeaponType::SNIPER) {
		float inaccuracy_random_spread_x = -inaccuracy_spread + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * inaccuracy_spread)));
		return df::Vector(target.getX() + inaccuracy + inaccuracy_random_spread_x, target.getY() + inaccuracy + inaccuracy_random_spread_y);
	}
	else {
		return df::Vector(target.getX(), target.getY() + inaccuracy + inaccuracy_random_spread_y);
	}
}

//Deal damage at the target position
//Prioritize hitting Minions first, then Boss weakpoints, then boss body
//This is called when a sniper is fired in scope mode (dealing instant damage)
//or a bullet just collided with an enemy at the target position
void Weapon::dealDamageAt(df::Vector target, bool drawTrail) {
	if (drawTrail) {
		df::Vector origin = getPosition() + (df::Vector(getBox().getHorizontal() / 2, -1.5f));
		df::Vector traveled = target - origin;
		int distance = traveled.getMagnitude();
		traveled.normalize();
		for (int i = 0; i <= distance; i++) {
			df::Vector scaled(traveled.getX(), traveled.getY());
			scaled.scale(i);
			DM.drawCh(origin + scaled, '.', df::YELLOW);
		}
	}

	//Signify the shot by creating an explosion at target position
	Explosion *p_explosion = new Explosion("explosion", 0);
	p_explosion->setPosition(target);
	// Play "explode" sound.
	df::Sound *p_sound = RM.getSound("explode");
	p_sound->play();

	df::ObjectList objects_at_target = WM.objectsAtPosition(target);
	bool hit = false;
	df::ObjectListIterator li(&objects_at_target);
	LM.writeLog("found %d objects at target\n", objects_at_target.getCount());

	//See if hitting minions
	li.first();
	while (!li.isDone()) {
		if (li.currentObject()->getType() == "Saucer") {
			if (dynamic_cast <Saucer *> (li.currentObject())->getEnemyType() == EnemyType::MINION) {
				hit = true;
				dynamic_cast <Saucer *> (li.currentObject())->takeDamage(target, damage);
				break;
			}
		}
		li.next();
	}

	//If didn't hit, see if hitting Weakpoints
	if (!hit) {
		li.first();
		while (!li.isDone()) {
			if (li.currentObject()->getType() == "Saucer") {
				if (dynamic_cast <Saucer *> (li.currentObject())->getEnemyType() == EnemyType::WEAKPOINT) {
					hit = true;
					dynamic_cast <WeakPoint *> (li.currentObject())->takeDamage(target, damage);
					break;
				}
			}
			li.next();
		}
	}

	//If didn't hit, see if hitting Boss body
	if (!hit) {
		li.first();
		while (!li.isDone()) {
			if (li.currentObject()->getType() == "Saucer") {
				if (dynamic_cast <Saucer *> (li.currentObject())->getEnemyType() == EnemyType::BOSS) {
					hit = true;
					dynamic_cast <Boss *> (li.currentObject())->takeDamage(target, damage);
					break;
				}
			}
			li.next();
		}
	}

}

bool Weapon::canShoot() {
	return ((fire_count_down == 0) && (!reloading));
}

void Weapon::fire(df::Vector target) {
	df::Vector origin = getPosition() + (df::Vector(getBox().getHorizontal() / 2, -1.5f));
	// See if time to fire.
	if ((fire_count_down > 0) || reloading) {
		return;
	}
	if (ammo_loaded == 0) {
		fire_count_down = fire_rate;
		RM.getSound("ammo_empty")->play();
		return;
	}
	fire_count_down = fire_rate;
	ammo_loaded--;
	last_shot_frame = GM.getStepCount();
	recoil = shot_recoil;

	//If scoping, deal instant damage at target, else fire Bullet towards target.
	if (weapon_type == WeaponType::SNIPER) {
		if (is_scoping) {
			dealDamageAt(target, true);
		}
		else {
			dealDamageAt(calculateInaccurateTarget(target), true);
		}
	}
	else {
		//Calculate the hit target based on inaccuracy
		df::Vector actualTarget = calculateInaccurateTarget(target);
		// Compute normalized vector to position, then scale by bullet speed.
		df::Vector v = actualTarget - origin;
		//df::Vector v = getDirection();
		v.normalize();
		v.scale(bullet_speed);
		printf("bullet velocity %f,%f\n", v.getX(), v.getY());
		Bullet *p = new Bullet(origin, bullet_sprite, this);
		p->setVelocity(v);
	}

	// Play "fire" sound.
	df::Sound *p_sound = df::ResourceManager::getInstance().getSound(weapon_name + "_fire");
	p_sound->play();
	hero->getReticle()->expand();
	df::Vector direction = (target - origin);
	direction.normalize();
	df::addParticles(60, 10, origin+df::Vector(0,0.3f), 0.1f, direction, 0.6f, 3.0f, 1.0f, 1.0f, 1.0f, 5, 10, (unsigned char)255, (char)255, (unsigned char)255, (unsigned char)100, (unsigned char)0, (unsigned char)255, df::ParticleClass::FIREWORK);

	//Increase inaccuracy for rifle
	if (weapon_type == WeaponType::RIFLE) {
		if (inaccuracy > -2.5) {
			inaccuracy -= 0.5f;
		}
		inaccuracy_spread = 0.2f;
	}

	//Finally, if ammo clip is empty then try to reload
	if (ammo_loaded == 0) {
		reload();
	}
}

int Weapon::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::STEP_EVENT) {
		step();
		return 1;
	}

	// If get here, have ignored this event.
	return 0;
}

void Weapon::toggleScope() {
	if ((weapon_type == WeaponType::SNIPER) && (!is_scoping)) {
		is_scoping = true;
	}
	else {
		is_scoping = false;
		df::ObjectList list_in_window = WM.getAllObjects();
		df::ObjectListIterator li(&list_in_window);
		li.first();
		while (!li.isDone()) {
			if (li.currentObject()->getType() != "Weapon") {
				li.currentObject()->setVisible(true);
			}             
			li.next();
		}
	}
}

void Weapon::step() {
	if (reloading) {
		float time = (float)clock->split() / (float)1000000;
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

	if ((weapon_type == WeaponType::RIFLE) && (inaccuracy != 0))
	{
		if ((GM.getStepCount() - last_shot_frame) >= 9) {
			inaccuracy = 0;
			inaccuracy_spread = 0;
		}
		else {
			inaccuracy += 0.05f;
		}
	}

	// Fire countdown.
	fire_count_down--;
	if (fire_count_down < 0)
		fire_count_down = 0;
	if ((weapon_type == WeaponType::SNIPER) && (fire_count_down == 30) && (!reloading)) {
		RM.getSound("awp_bolt")->play();
	}


	recoil -= 0.35f;
	if (recoil < 0) recoil = 0;

	setPosition(hero->getPosition() + df::Vector(3, 1) + df::Vector(-recoil, 0));
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
	RM.getSound(weapon_name + "_reload")->play();
	reloading = true;
	clock->delta();
}

void Weapon::refillAmmo() {
	ammo_loaded = ammo_loaded_max;
	ammo_backup = ammo_backup_max;
}

void Weapon::pickPower(){
    damage = newDamage;
    shootingcount--;

    if (shootingcount <= 0){
        newDamage = damage;
    }
}

int Weapon::getAmmoLoaded() {
	return ammo_loaded;
}

int Weapon::getAmmoBackup() {
	return ammo_backup;
}

WeaponType Weapon::getWeaponType() {
	return weapon_type;
}

bool Weapon::isScoping() {
	return is_scoping;
}