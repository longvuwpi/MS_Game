#pragma once
#include "Saucer.h"
#include "Boss.h"

class Boss;

class WeakPoint : public Saucer {
private:
	Boss *owner;
	df::Vector relative_position;
	int damage_multiplier;
public:
	WeakPoint(Boss *owner, df::Vector relativePosition, int damage_multiplier, int health);
	int eventHandler(const df::Event *p_e);
	void takeDamage(df::Vector at, int damage);
};