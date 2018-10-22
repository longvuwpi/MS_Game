#include "WeakPoint.h"
#include "EventBossTookDamage.h"

#include "EventStep.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "ObjectList.h"
#include "WorldManager.h"
#include "DisplayManager.h"

WeakPoint::WeakPoint(df::Vector relativePosition, int damageMultiplier, int health) : Saucer(health, 0, 0) {
	relative_position = relativePosition;
	damage_multiplier = damageMultiplier;
	setEnemyType(EnemyType::WEAKPOINT);
	setSolidness(df::SOFT);

	df::Sprite *p_temp_sprite = RM.getSprite("boss-weakpoint");
	if (!p_temp_sprite)
		LM.writeLog("Saucer::Saucer(): Warning! Sprite '%s' not found", "saucer");
	else {
		setSprite(p_temp_sprite);
		setSpriteSlowdown(60);
	}

}

void WeakPoint::takeDamage(df::Vector at, int damage) {
	int damage_taken = damage * damage_multiplier;
	Saucer::takeDamage(at, damage_taken);
	EventBossTookDamage* eventBossTookDamage = new EventBossTookDamage(at, damage_taken);
	WM.onEvent(eventBossTookDamage);
}

void WeakPoint::zeroHP() {
	health = 0;
	Saucer::die();
}

df::Vector WeakPoint::getRelativePosition() {
	return relative_position;
}

void WeakPoint::draw() {
	Object::draw();
	DM.drawString(getPosition(), std::to_string(health), df::CENTER_JUSTIFIED, df::GREEN);
}