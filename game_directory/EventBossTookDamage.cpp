#include "EventBossTookDamage.h"

EventBossTookDamage::EventBossTookDamage(df::Vector pos, int dmg) {
	position = pos;
	damage = dmg;
	setType(BOSS_TOOK_DAMAGE_EVENT);
}

int EventBossTookDamage::getDamage() const {
	return damage;
}

df::Vector EventBossTookDamage::getDamagePosition() const {
	return position;
}