#pragma once
#include "Saucer.h"
#include "Boss.h"

class Boss;

class WeakPoint : public Saucer {
private:
	df::Vector relative_position;
	int damage_multiplier;
public:
	WeakPoint(df::Vector relativePosition, int damage_multiplier, int health);
	void takeDamage(df::Vector at, int damage) override;
	void zeroHP();
	df::Vector getRelativePosition();
	void draw();
};