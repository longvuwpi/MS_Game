#pragma once

#include "Vector.h"
#include "Saucer.h"

class Saucer;

class EnemyAttack {
private:
	Saucer * owner;
	int fire_count_down;
public:
	EnemyAttack(Saucer *saucer);
	void attack();
	void fire(float bulletSpeed);
};