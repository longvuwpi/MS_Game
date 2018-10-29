#pragma once
#include "Saucer.h"

#include "ObjectList.h"

class WeakPoint;

class Boss : public Saucer {
private:
	df::ObjectList weakpoint_list;
	void step() override;
public:
	//no parameters for now
	Boss(int health);
	void createWeakPoint(df::Vector relative_position, int damage_multiplier, int weakpoint_health); //The relative position of the weakpoint to the boss
	int eventHandler(const df::Event *p_e);
	void die() override;
	void draw();
};