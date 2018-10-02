//
// Hero.cpp
//
#include <iostream>

#include <stdio.h>

// Engine includes.
#include "EventMouse.h"
#include "EventStep.h"
#include "EventView.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"

// Game includes.
#include "Bullet.h"
#include "EventNuke.h"
#include "Explosion.h"
#include "GameOver.h"
#include "Hero.h"
#include "EventPlayerFalling.h"
#include "EventPlayerJumping.h"

int moveSpeed = 2;
int jumpHeight = 10;

Hero::Hero() {

	// Link to "ship" sprite.
	walk_sprite = RM.getSprite("character-walk");
	if (!walk_sprite)
		LM.writeLog("Hero::Hero(): Warning! Sprite '%s' not found", "ship");
	else {
		setWalkingSprite();
	}

	duck_sprite = RM.getSprite("character-duck");
	if (!duck_sprite)
		LM.writeLog("duck sprite not found");

	// Player controls hero, so register for input events.
	registerInterest(df::KEYBOARD_EVENT);
	registerInterest(df::MOUSE_EVENT);

	// Need to update rate control each step.
	registerInterest(df::STEP_EVENT);

	// Set object type.
	setType("Hero");

	// Set starting location.
	//df::Vector p(7, WM.getBoundary().getVertical()/2);
	df::Vector p(7, 52);
	setPosition(p);

	// Create reticle for firing bullets and aiming.
	p_reticle = new Reticle();
	p_reticle->draw();

	// Set attributes that control actions.
	move_slowdown = 1;
	move_countdown = move_slowdown;
	fire_slowdown = 5;
	fire_countdown = fire_slowdown;
	nuke_count = 1;

	isDucking = false;
}

void Hero::setWalkingSprite() {
	setSprite(walk_sprite);
	setSpriteSlowdown(5);  // 1/3 speed animation.
	setTransparency();	   // Transparent sprite.
}

void Hero::setDuckingSprite() {
	setSprite(duck_sprite);
}

Hero::~Hero() {

	// Create GameOver object.
	GameOver *p_go = new GameOver;

	// Make big explosion.
	for (int i = -8; i <= 8; i += 5) {
		for (int j = -5; j <= 5; j += 3) {
			df::Vector temp_pos = this->getPosition();
			temp_pos.setX(this->getPosition().getX() + i);
			temp_pos.setY(this->getPosition().getY() + j);
			Explosion *p_explosion = new Explosion;
			p_explosion->setPosition(temp_pos);
		}
	}

	// Mark Reticle for deletion.
	WM.markForDelete(p_reticle);
}

// Handle event.
// Return 0 if ignored, else 1.
int Hero::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::COLLISION_EVENT) {
		//printf("collided with hero");
		const df::EventCollision *p_collision_event = dynamic_cast <df::EventCollision const *> (p_e);
		if (p_collision_event->getObject1()->getType() == "Platform") {
			p_OnPlatform = dynamic_cast <Platform *> (p_collision_event->getObject1());
		} else if (p_collision_event->getObject2()->getType() == "Platform") {
			p_OnPlatform = dynamic_cast <Platform *> (p_collision_event->getObject2());
		}
			return 1;
	}

	if (p_e->getType() == df::KEYBOARD_EVENT) {
		const df::EventKeyboard *p_keyboard_event = dynamic_cast <const df::EventKeyboard *> (p_e);
		kbd(p_keyboard_event);
		return 1;
	}

	if (p_e->getType() == df::MOUSE_EVENT) {
		const df::EventMouse *p_mouse_event = dynamic_cast <const df::EventMouse *> (p_e);
		mouse(p_mouse_event);
		return 1;
	}

	if (p_e->getType() == df::STEP_EVENT) {
		step();
		return 1;
	}

	// If get here, have ignored this event.
	return 0;
}

// Take appropriate action according to mouse action.
void Hero::mouse(const df::EventMouse *p_mouse_event) {
	//switch (p_mouse_event->getMouseAction()) {
	//case df::CLICKED:
	//	std::cout << "Clicked\n";
	//	break;
	//case df::PRESSED:
	//	std::cout << "Pressed\n";
	//	break;
	//default:
	//	break;
	//}
	// Pressed button?
	if (((p_mouse_event->getMouseAction() == df::CLICKED) || (p_mouse_event->getMouseAction() == df::PRESSED)) &&
		(p_mouse_event->getMouseButton() == df::Mouse::LEFT))
		fire((*(new df::Vector(WM.getView().getCorner().getX(), 0))) + p_mouse_event->getMousePosition());
}

// Take appropriate action according to key pressed.
void Hero::kbd(const df::EventKeyboard *p_keyboard_event) {

	switch (p_keyboard_event->getKey()) {
	case df::Keyboard::W:       // up
		if (p_keyboard_event->getKeyboardAction() == df::KEY_DOWN)
			//move(-1);
		break;
	case df::Keyboard::S:       // down
		if (p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
			setDuckingSprite();
			isDucking = true;
		}
		else if (p_keyboard_event->getKeyboardAction() == df::KEY_RELEASED) {
			isDucking = false;
			setWalkingSprite();
		}
		break;
	case df::Keyboard::A:       // left
		if (p_keyboard_event->getKeyboardAction() == df::KEY_DOWN)
			move(-moveSpeed,0);
		break;
	case df::Keyboard::D:       // right
		if (p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
			move(moveSpeed, 0);
		}
		break;
	case df::Keyboard::SPACE:   // nuke!
		if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED)
			if (isDucking) {
				p_OnPlatform->setSolidness(df::SOFT);
			}
			else {
				jump();
			}
		break;
	case df::Keyboard::Q:        // quit
		if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) {
			df::WorldManager &world_manager = df::WorldManager::getInstance();
			world_manager.markForDelete(this);
		}
		break;
	};
	//std::cout << "Hero position: " << getPosition().getX() << "," << getPosition().getY() << "\n";
	//std::cout << WM.getViewFollowing()->getType();
	return;
}

// Move up or down.
void Hero::move(int dx, int dy) {

	// See if time to move.
	if (move_countdown > 0)
		return;
	move_countdown = move_slowdown;

	// If stays on window, allow move.
	df::Vector new_pos(getPosition().getX()+dx, getPosition().getY() + dy);
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	if ((new_pos.getY() > 3) &&
		(new_pos.getY() < world_manager.getBoundary().getVertical() - 1))
		world_manager.moveObject(this, new_pos);

	WM.setViewPosition(new_pos);
}

// Fire bullet towards target.
void Hero::fire(df::Vector target) {

	// See if time to fire.
	if (fire_countdown > 0)
		return;
	fire_countdown = fire_slowdown;

	// Fire Bullet towards target.
	// Compute normalized vector to position, then scale by speed (1).
	df::Vector v = target - getPosition();
	//df::Vector v = getDirection();
	v.normalize();
	v.scale(5);
	printf("bullet velocity %f,%f\n", v.getX(), v.getY());
	Bullet *p = new Bullet(getPosition());
	p->setVelocity(v);

	// Play "fire" sound.
	df::Sound *p_sound = df::ResourceManager::getInstance().getSound("fire");
	p_sound->play();

	printf("hero pos %f,%f\n", getPosition().getX(), getPosition().getY());
	printf("hero box %s\n", isCentered() ? "is centered" : "is not centered");
}

// Decrease rate restriction counters.
void Hero::step() {

	// Move countdown.
	move_countdown--;
	if (move_countdown < 0)
		move_countdown = 0;

	// Fire countdown.
	fire_countdown--;
	if (fire_countdown < 0)
		fire_countdown = 0;

	if (getVelocity().getY() < 1) {
		if ((getVelocity().getY() <= 0) && ((getVelocity().getY() + 1.0f) > 0)) {
			df::Vector player_pos = getPosition();
			EventPlayerFalling* eventPlayerFalling = new EventPlayerFalling(player_pos);
			WM.onEvent(eventPlayerFalling);
		}
		setVelocity(getVelocity() + *(new df::Vector(0, 2.0f)));
	}
}

// Send "nuke" event to all objects.
void Hero::jump() {

	//// Check if nukes left.
	//if (!nuke_count) 
	//  return;
	//nuke_count--;

	//// Create "nuke" event and send to interested Objects.
	//df::WorldManager &world_manager = df::WorldManager::getInstance();
	//EventNuke nuke;
	//world_manager.onEvent(&nuke);

	//// Send "view" event do decrease number of nukes to interested ViewObjects.
	//df::EventView ev("Nukes", -1, true);
	//world_manager.onEvent(&ev);

	//// Play "nuke" sound.
	//df::Sound *p_sound = df::ResourceManager::getInstance().getSound("nuke");
	//p_sound->play();
	setVelocity(df::Vector(0, -jumpHeight));

	df::Vector player_pos = getPosition();
	EventPlayerJumping* eventPlayerJumping = new EventPlayerJumping(player_pos);
	WM.onEvent(eventPlayerJumping);
}
