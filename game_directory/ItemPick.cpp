//
#include <iostream>
#include <stdio.h>
#include <math.h>

#include "ItemPick.h"
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

ItemPick::ItemPick() {
	setType("Item");

	df::Sprite *p_current_sprite = RM.getSprite(Item_name);
	if (!p_current_sprite)
		LM.writeLog("Item sprite not found");
	else setSprite(p_current_sprite);

	setSolidness(df::SOFT);
	registerInterest(df::KEYBOARD_EVENT);

    switch (item_type) {
    case ItemType::AMMOREFILL:
        
        break;
    case ItemType::HEALTH:
        
        break;
    }
    
}

int ItemPick::eventHandler(const df::Event *p_e) {
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

void ItemPick::draw() {
	if (isVisible()) {
		df::Object::draw();
		DM.drawString(getPosition() - df::Vector(0, 1 + (getSprite()->getHeight() / 2)) - df::Vector(0, sin((float)GM.getStepCount() / 5) / 2), "Press E", df::CENTER_JUSTIFIED, df::RED);
	}
}
