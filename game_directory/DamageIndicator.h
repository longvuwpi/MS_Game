#pragma once
#include "Object.h"
#include "Event.h"
#include "Vector.h"

class DamageIndicator : public df::Object {
private:
	int life_time;
	int damage;
public:
	DamageIndicator(df::Vector at, int damage);
	int eventHandler(const df::Event *p_e);
	void draw();
};