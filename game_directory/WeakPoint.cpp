#include "WeakPoint.h"

#include "EventStep.h"

WeakPoint::WeakPoint(Boss *boss, df::Vector relativePosition, int damageMultiplier, int health) : Saucer(health, 0, 0) {
	owner = boss;
	relative_position = relativePosition;
	damage_multiplier = damageMultiplier;
	setEnemyType(EnemyType::WEAKPOINT);
	setSolidness(df::SOFT);
	registerInterest(df::STEP_EVENT);
}

int WeakPoint::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::STEP_EVENT) {
		setPosition(owner->getPosition() + relative_position);
		return 1;
	}

	// If get here, have ignored this event.
	return 0;
}

void WeakPoint::takeDamage(df::Vector at, int damage) {
	int damage_taken = damage * damage_multiplier;
	Saucer::takeDamage(at, damage_taken);
	owner->takeDamage(at, damage_taken);
}