#include "Boss.h"
#include "WeakPoint.h"
#include "EventBossTookDamage.h"

#include "ResourceManager.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "EventStep.h"
#include "DisplayManager.h"

//Boss is not yet doing damage and moving
Boss::Boss(int health) : Saucer::Saucer(health,0,0) {
	setEnemyType(EnemyType::BOSS);
	df::Sprite *p_temp_sprite = RM.getSprite("boss");

	if (!p_temp_sprite)
		LM.writeLog("Saucer::Saucer(): Warning! Sprite '%s' not found", "boss");
	else {
		setSprite(p_temp_sprite);
	}

	registerInterest(BOSS_TOOK_DAMAGE_EVENT);
}

int Boss::eventHandler(const df::Event *p_e) {
	Saucer::eventHandler(p_e);
	if (p_e->getType() == BOSS_TOOK_DAMAGE_EVENT) {
		const EventBossTookDamage *p_boss_took_dmg_event = dynamic_cast <EventBossTookDamage const *> (p_e);
		Saucer::takeDamage(p_boss_took_dmg_event->getDamagePosition(), p_boss_took_dmg_event->getDamage());
		return 1;
	}

	if (p_e->getType() == df::STEP_EVENT) {
		Boss::step();
		return 1;
	}

	return 0;
}

void Boss::die() {
	if (!weakpoint_list.isEmpty()) {
		df::ObjectListIterator li(&weakpoint_list);
		li.first();
		while (!li.isDone()) {
			dynamic_cast <WeakPoint *> (li.currentObject())->zeroHP();
			li.next();
		}
	}
	Saucer::die();
}

void Boss::createWeakPoint(df::Vector relative_position, int damage_multiplier, int weakpoint_health) {
	weakpoint_list.insert(new WeakPoint(relative_position, damage_multiplier, weakpoint_health));
}

void Boss::step() {
	df::ObjectListIterator li(&weakpoint_list);
	df::ObjectList to_remove;
	li.first();
	while (!li.isDone()) {
		WeakPoint * current_weakPoint = dynamic_cast <WeakPoint *> (li.currentObject());
		if (current_weakPoint->getHealth() <= 0) {
			to_remove.insert(current_weakPoint);
		}
		else {
			current_weakPoint->setPosition(getPosition() + dynamic_cast <WeakPoint *> (li.currentObject())->getRelativePosition());
		}
		li.next();
	}

	if (!to_remove.isEmpty()) {
		df::ObjectListIterator lir(&to_remove);
		lir.first();
		while (!lir.isDone()) {
			weakpoint_list.remove(lir.currentObject());
			lir.next();
		}
	}
}

void Boss::draw() {
	Object::draw();
	DM.drawString(getPosition(), std::to_string(health), df::CENTER_JUSTIFIED, df::GREEN);
}