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

ItemPick::ItemPick(std::string ItemName, ItemType itemType) {

    item_name = ItemName;
    item_type = itemType;
	setType("Item");

	df::Sprite *p_current_sprite = RM.getSprite(item_name);
	if (!p_current_sprite)
		LM.writeLog("Item sprite not found");
	else setSprite(p_current_sprite);

	setSolidness(df::SOFT);
	registerInterest(df::KEYBOARD_EVENT);

    switch (item_type) {
    case ItemType::AMMOREFILL:
    {    
        //setType("AmmoRefill");

        df::Sprite *ammo_refill_sprite = RM.getSprite("ammo-refill");
        if (!ammo_refill_sprite){
            LM.writeLog("amoo refill sprite not found");
            }
        else {
            setSprite(ammo_refill_sprite);
        }

        setSolidness(df::SOFT);
        registerInterest(df::KEYBOARD_EVENT);

        break;
    }
    case ItemType::HEALTHPICK:
    {   
        //setType("HealthPick");

        df::Sprite *health_pick_sprite = RM.getSprite("health-pick");
        if (!health_pick_sprite){
            LM.writeLog("health pick sprite not found");
            }
        else {
            setSprite(health_pick_sprite);
        }

        setSolidness(df::SOFT);
        registerInterest(df::KEYBOARD_EVENT);

        break;
    }
    case ItemType::POWERUP:

        df::Sprite *power_pick_sprite = RM.getSprite("power-pick");
        if (!power_pick_sprite){
            LM.writeLog("power pick sprite not found");
            }
        else {
            setSprite(power_pick_sprite);
        }

        setSolidness(df::SOFT);
        registerInterest(df::KEYBOARD_EVENT);
        
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
        ItemType(HEALTHPICK);
		return 1;
	}

    else if(p_e->getType() == df::KEYBOARD_EVENT) {
        const df::EventKeyboard *p_keyboard_event = dynamic_cast <const df::EventKeyboard *> (p_e);
        switch (p_keyboard_event->getKey()) {
        case df::Keyboard::E:
            if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) {
                df::ObjectList collisions = WM.isCollision(this, getPosition());
                df::ObjectList hero = WM.objectsOfType("Hero");
                df::ObjectListIterator li(&hero);
                li.first();
                if (collisions.remove(li.currentObject()) == 0) {
                    RM.getSound("pickup1")->play();
                    RM.getSound("pickup2")->play();
                    (dynamic_cast <Hero *> (li.currentObject()))->refillAmmo();
                    std::cout << "collided with hero";
                }
            }
            break;
        }
        ItemType(AMMOREFILL);
        return 1;
    }

    else if(p_e->getType() == df::KEYBOARD_EVENT) {
        const df::EventKeyboard *p_keyboard_event = dynamic_cast <const df::EventKeyboard *> (p_e);
        switch (p_keyboard_event->getKey()) {
        case df::Keyboard::P:
            if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) {
                df::ObjectList collisions = WM.isCollision(this, getPosition());
                df::ObjectList hero = WM.objectsOfType("Hero");
                df::ObjectListIterator li(&hero);
                li.first();
                if (collisions.remove(li.currentObject()) == 0) {
                    RM.getSound("pickup4")->play();
                    (dynamic_cast <Hero *> (li.currentObject()))->pickPower();
                    std::cout << "collided with hero";
                }
            }
            break;
        }
        ItemType(POWERUP);
        return 1;
    }
	else {
        return 0;
    }
}

void ItemPick::draw() {
	if (isVisible()) {
		df::Object::draw();
		DM.drawString(getPosition() - df::Vector(0, 1 + (getSprite()->getHeight() / 2)) - df::Vector(0, sin((float)GM.getStepCount() / 5) / 2), "Press E", df::CENTER_JUSTIFIED, df::RED);
	}
}

std::string ItemPick::getItemName() {
    return item_name;
}

ItemType ItemPick::getItemType() {
    return item_type;
}


