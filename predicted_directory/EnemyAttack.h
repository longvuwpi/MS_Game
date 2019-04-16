#pragma once

#include "Vector.h"
#include "Saucer.h"

class Saucer;

class EnemyAttack {
private:
	Saucer * owner;
	int fire_count_down;
	int start_frame;
public:
	EnemyAttack(Saucer *saucer);
	void attack();
	void fireAtHero(float bulletSpeed);
	void customFire(float bulletSpeed, df::Vector from, df::Vector to);
	df::Vector getTrackedHeroPos();
};