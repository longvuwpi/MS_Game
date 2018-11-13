//
#include <iostream>
#include <stdio.h>
#include <math.h>

#include "HealthPick.h"
#include "Hero.h"

#include "ResourceManager.h"
#include "WorldManager.h"
#include "LogManager.h"
#include "GameManager.h"
#include "DisplayManager.h"
#include "Object.h"
#include "ObjectList.h"
#include "ObjectListIterator.h"
#include "EventKeyboard.h"
#include "Sprite.h"

HealthPick::HealthPick() {
	setType("HealthPick");

	df::Sprite *health_pick_sprite = RM.getSprite("health-pick");
	if (!health_pick_sprite)
		LM.writeLog("health pick sprite not found");
	else setSprite(health_pick_sprite);

	setSolidness(df::SOFT);
	registerInterest(df::KEYBOARD_EVENT);
}

int HealthPick::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::KEYBOARD_EVENT) {
		const df::EventKeyboard *p_keyboard_event = dynamic_cast <const df::EventKeyboard *> (p_e);
		switch (p_keyboard_event->getKey()) {
		case df::Keyboard::E:
			if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) {
				df::ObjectList collisions = WM.isCollision(this, getPosition());
				df::ObjectList hero = WM.objectsOfType("Hero");
				df::ObjectListIterator li(&hero);
				li.first();
				if (collisions.remove(li.currentObject()) == 0) {
					RM.getSound("pickup3")->play();
					(dynamic_cast <Hero *> (li.currentObject()))->pickHealth();
					std::cout << "collided with hero";
				}
			}
			break;
		}
		return 1;
	}

	return 0;
}

void HealthPick::draw() {
	if (isVisible()) {
		df::Object::draw();
		DM.drawString(getPosition() - df::Vector(0, 1 + (getSprite()->getHeight() / 2)) - df::Vector(0, sin((float)GM.getStepCount() / 5) / 2), "Press E", df::CENTER_JUSTIFIED, df::RED);
	}
}