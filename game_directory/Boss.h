#pragma once
#include "Saucer.h"

class WeakPoint;

class Boss : public Saucer {
public:
	//no parameters for now
	Boss(int health);
	void createWeakPoint(df::Vector relative_position, int damage_multiplier, int weakpoint_health); //The relative position of the weakpoint to the boss
};