#include <stdio.h>
#include <iostream>

#include "ObjectList.h"
#include "WorldManager.h"
#include "GameManager.h"

#include "Bullet.h"
#include "EnemyAttack.h"

int fire_rate = 90;

EnemyAttack::EnemyAttack(Saucer *saucer) {
	owner = saucer;
	fire_count_down = fire_rate;
	start_frame = GM.getStepCount();
}

void EnemyAttack::attack() {
	switch (owner->getAttackType()) {
	case 0:
		break;
	case 1:
	{
		fire_count_down--;
		if (fire_count_down <= 0)
		{
			fire_count_down = fire_rate;
			fireAtHero(2);
		}
		break;
	}
	case 2:
	{
		int current = GM.getStepCount() - start_frame;
		if (current % 120 == 0) {
			df::ObjectList object_list = WM.objectsOfType("Hero");
			if (object_list.getCount() > 0) {
				df::ObjectListIterator li(&object_list);
				li.first();
				df::Vector hero_pos = li.currentObject()->getPosition();
				df::Vector origin = owner->getPosition();
				customFire(1, origin + df::Vector(0, 1.0f), hero_pos);
				customFire(1, origin + df::Vector(0, -1.0f), hero_pos);
			}
		}
	}
	default:
		break;
	}

	return;
}

void EnemyAttack::fireAtHero(float bulletSpeed) {
	df::ObjectList object_list = WM.objectsOfType("Hero");
	if (object_list.getCount() > 0) {
		df::ObjectListIterator li(&object_list);
		li.first();
		df::Vector hero_pos = li.currentObject()->getPosition();

		std::cout << "Hero pos: (" << hero_pos.getX() << "," << hero_pos.getY() << ")\n";

		df::Vector origin = owner->getPosition();

		// Fire Bullet towards target.
		// Compute normalized vector to position, then scale by speed.
		//df::Vector v = hero_pos - origin;
		//v.normalize();
		//v.scale(bulletSpeed);
		//Bullet *p = new Bullet(origin, owner->getBulletSprite(), owner->getDamage(), owner->getBulletRadius());
		//p->setVelocity(v);
		customFire(bulletSpeed, origin, hero_pos);
	}
}

void EnemyAttack::customFire(float bulletSpeed, df::Vector from, df::Vector to) {
	df::Vector v = to - from;

	//df::Vector v = getDirection();
	v.normalize();
	v.scale(bulletSpeed);
	Bullet *p = new Bullet(from, owner->getBulletSprite(), owner->getDamage(), owner->getBulletRadius());
	p->setVelocity(v);

}