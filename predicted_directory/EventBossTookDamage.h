#pragma once
#include "Event.h"
#include "Vector.h"

const std::string BOSS_TOOK_DAMAGE_EVENT = "bosstookdamage";

class EventBossTookDamage : public df::Event {
private:
	df::Vector position;
	int damage;
public:
	EventBossTookDamage(df::Vector positionm, int damage);
	int getDamage() const;
	df::Vector getDamagePosition() const;
};
