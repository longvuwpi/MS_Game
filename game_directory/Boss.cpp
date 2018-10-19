#include "Boss.h"
#include "WeakPoint.h"

//Boss is not yet doing damage
Boss::Boss(int health) : Saucer::Saucer(health,0,0) {
	setEnemyType(EnemyType::BOSS);
}

void Boss::createWeakPoint(df::Vector relative_position, int damage_multiplier, int weakpoint_health) {
	new WeakPoint(this, relative_position, damage_multiplier, weakpoint_health);
}
