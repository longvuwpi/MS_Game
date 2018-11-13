#include <stdio.h>
#include <iostream>

#include "ObjectList.h"
#include "WorldManager.h"

#include "Bullet.h"
#include "EnemyAttack.h"

int fire_rate = 90;

EnemyAttack::EnemyAttack(Saucer *saucer) {
	owner = saucer;
	fire_count_down = fire_rate;
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
			fire(2);
		}
		break;
	}
	default:
		break;
	}

	return;
}

void EnemyAttack::fire(float bulletSpeed) {
	df::ObjectList object_list = WM.objectsOfType("Hero");
	if (object_list.getCount() > 0) {
		df::ObjectListIterator li(&object_list);
		li.first();
		df::Vector hero_pos = li.currentObject()->getPosition();

		std::cout << "Hero pos: (" << hero_pos.getX() << "," << hero_pos.getY() << ")\n";

		df::Vector origin = owner->getPosition();

		// Fire Bullet towards target.
		// Compute normalized vector to position, then scale by speed.
		df::Vector v = hero_pos - origin;

		//df::Vector v = getDirection();
		v.normalize();
		v.scale(bulletSpeed);
		Bullet *p = new Bullet(origin, owner->getBulletSprite(), owner->getDamage(), owner->getBulletRadius());
		p->setVelocity(v);
	}
}