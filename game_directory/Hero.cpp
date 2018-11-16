//
// Hero.cpp
//
#include <iostream>
#include <stdio.h>

// Engine includes.
#include "EventCollision.h"
#include "EventMouse.h"
#include "EventStep.h"
#include "EventView.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "ObjectListIterator.h"
#include "DisplayManager.h"

// Game includes.
#include "Weapon.h"
#include "Bullet.h"
#include "BulletTrail.h"
#include "EventNuke.h"
#include "Explosion.h"
#include "GameOver.h"
#include "Hero.h"
#include "EventPlayerFalling.h"
#include "EventPlayerJumping.h"
#include "Saucer.h"
#include "DamageIndicator.h"

int moveSpeed = 2;
int jumpHeight = 10;

Hero::Hero() {

	// Link to "ship" sprite.
	walk_sprite = RM.getSprite("character-walk");
	if (!walk_sprite)
		LM.writeLog("Hero::Hero(): Warning! Sprite '%s' not found", "ship");

	duck_sprite = RM.getSprite("character-duck");
	if (!duck_sprite)
		LM.writeLog("duck sprite not found");

	setWalkingSprite();

	// Player controls hero, so register for input events.
	registerInterest(df::KEYBOARD_EVENT);
	registerInterest(df::MSE_EVENT);

	// Need to update rate control each step.
	registerInterest(df::STEP_EVENT);

	// Set object type.
	setType("Hero");

	// Set starting location.
	//df::Vector p(7, WM.getBoundary().getVertical()/2);
	df::Vector p(7, 52);
	setPosition(p);

	//p_reticle->draw();

	// Set attributes that control actions.
	move_slowdown = 1;
	move_countdown = move_slowdown;
	fire_slowdown = 5;
	fire_countdown = fire_slowdown;
	nuke_count = 1;
	jump_max = 1;
	jump_count = jump_max;
	max_health = 50;
	health = 50;
	isDucking = false;
	p_OnPlatform = NULL;

	setAcceleration(df::Vector(0, 2));

	//Set up weapons
	Weapon *ak47 = new Weapon("AK47", WeaponType::RIFLE, this, 10, 3, 30, 90, 8, false, 0, 0, 2.5f);
	weapon_list.insert(ak47);

	Weapon *awp = new Weapon("AWP", WeaponType::SNIPER, this, 18, 40, 10, 30, 30, false, 0, 0, 2.5f);
	weapon_list.insert(awp);
	awp->setVisible(false);

	Weapon *grenade_launcher = new Weapon("GrenadeLauncher", WeaponType::LAUNCHER, this, 8, 40, 1, 30, 10, true, 0.4f, 20, 1.5f);
	weapon_list.insert(grenade_launcher);
	grenade_launcher->setVisible(false);

	weapon_selector = new df::ObjectListIterator(&weapon_list);
	weapon_selector->first();

	//Weapon view
	weapon_view = new df::ViewObject; // Count of nukes.
	weapon_view->setLocation(df::TOP_LEFT);
	weapon_view->setViewString(getCurrentWeapon()->getWeaponName() + ":");
	weapon_view->setColor(df::YELLOW);

	// Create reticle for firing bullets and aiming.
	p_reticle = new Reticle(this);
}

void Hero::setWalkingSprite() {
	setSprite(walk_sprite);
	setSpriteSlowdown(5);  // 1/3 speed animation.
	setTransparency();	   // Transparent sprite.
	setPosition(getPosition() + df::Vector(0, -0.5 + (duck_sprite->getHeight() / 2) - (walk_sprite->getHeight() / 2)));
	setCentered(true);
}

void Hero::setDuckingSprite() {
	setSprite(duck_sprite);
	setPosition(getPosition() + df::Vector(0, -0.5 + (walk_sprite->getHeight() / 2) - (duck_sprite->getHeight() / 2)));
	setCentered(true);
}

Hero::~Hero() {

	// Make big explosion.
	for (int i = -8; i <= 8; i += 5) {
		for (int j = -5; j <= 5; j += 3) {
			df::Vector temp_pos = this->getPosition();
			temp_pos.setX(this->getPosition().getX() + i);
			temp_pos.setY(this->getPosition().getY() + j);
			Explosion *p_explosion = new Explosion("explosion", 0);
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
			Platform *platform = dynamic_cast <Platform *> (p_collision_event->getObject1());
			landedOn(platform);
		}
		else if (p_collision_event->getObject2()->getType() == "Platform") {
			Platform *platform = dynamic_cast <Platform *> (p_collision_event->getObject2());
			landedOn(platform);
		}
		hit(p_collision_event);
		return 1;
	}

	if (p_e->getType() == df::KEYBOARD_EVENT) {
		const df::EventKeyboard *p_keyboard_event = dynamic_cast <const df::EventKeyboard *> (p_e);
		kbd(p_keyboard_event);
		return 1;
	}

	if (p_e->getType() == df::MSE_EVENT) {
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

void Hero::landedOn(Platform *platform) {
	if (p_OnPlatform == NULL) {
		p_OnPlatform = platform;

		std::cout << "Landed on platform, hero pos: (" << getPosition().getX() << "," << getPosition().getY() << ")\n";
		//if player is hovering above platform, set player to be exactly on platform
		df::Vector delta(0, -0.5 + (p_OnPlatform->getPosition().getY() - getPosition().getY()) - ((p_OnPlatform->getSprite()->getHeight() + getSprite()->getHeight()) / 2));
		if (delta.getY() > 0)
			setPosition(getPosition() + delta);

		jump_count = jump_max;

		setCentered(true);

		return;
	}
}

// Take appropriate action according to mouse action.
void Hero::mouse(const df::EventMouse *p_mouse_event) {
	// Pressed button?
	if (((p_mouse_event->getMouseAction() == df::CLICKED) || (p_mouse_event->getMouseAction() == df::PRESSED)) &&
		(p_mouse_event->getMouseButton() == df::Mouse::LEFT)) {
		fire(p_reticle->getPosition());
	} else if ((p_mouse_event->getMouseAction() == df::CLICKED) &&
		(p_mouse_event->getMouseButton() == df::Mouse::RIGHT)) {
		getCurrentWeapon()->toggleScope();
	}
}

// Take appropriate action according to key pressed.
void Hero::kbd(const df::EventKeyboard *p_keyboard_event) {

	switch (p_keyboard_event->getKey()) {
	case df::Keyboard::W:       // up
		if (p_keyboard_event->getKeyboardAction() == df::KEY_DOWN)
			//move(-1);
			break;
	case df::Keyboard::S:       // down
		if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) {
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
			move(-moveSpeed, 0);
		break;
	case df::Keyboard::D:       // right
		if (p_keyboard_event->getKeyboardAction() == df::KEY_DOWN) {
			move(moveSpeed, 0);
		}
		break;
	case df::Keyboard::SPACE:   // jump!
		if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED)
			if (jump_count > 0) {
				if (isDucking) {
					p_OnPlatform->setSolidness(df::SOFT);
				}
				else {
					jump();
				}
				jump_count--;
			}
		break;
	case df::Keyboard::F:       // change weapon
		if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED)
		{
			if (getCurrentWeapon()->isScoping()) {
				getCurrentWeapon()->toggleScope();
			}
			getCurrentWeapon()->setVisible(false);
			weapon_selector->next();
			if (weapon_selector->isDone()) {
				weapon_selector->first();
			}
			getCurrentWeapon()->setVisible(true);
			weapon_view->setViewString(getCurrentWeapon()->getWeaponName() + ":");
		}
		//move(-1);
		break;
	case df::Keyboard::R:   
		if (p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) {
			reload();
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
	df::Vector new_pos(getPosition().getX() + dx, getPosition().getY() + dy);
	df::WorldManager &world_manager = df::WorldManager::getInstance();
	if ((new_pos.getY() > 3) &&
		(new_pos.getY() < world_manager.getBoundary().getVertical() - 1))
		world_manager.moveObject(this, new_pos);
}

Weapon *Hero::getCurrentWeapon() {
	return (dynamic_cast <Weapon*> (weapon_selector->currentObject()));
}

// Fire bullet towards target.
void Hero::fire(df::Vector target) {
	getCurrentWeapon()->fire(target);
}

void Hero::reload() {
	getCurrentWeapon()->reload();
}

// Decrease rate restriction counters.
void Hero::step() {
	//weapon_view->setValue(getCurrentWeapon()->getAmmo());
	weapon_view->setViewString(getCurrentWeapon()->getWeaponName() + ":" + std::to_string(getCurrentWeapon()->getAmmoLoaded()) + "/" + std::to_string(getCurrentWeapon()->getAmmoBackup()));

	// Move countdown.
	move_countdown--;
	if (move_countdown < 0)
		move_countdown = 0;

	//Check to see if the hero is on a platform.
	df::ObjectList collisions = WM.isCollision(this, getPosition() + df::Vector(0, 0.5));
	if ((p_OnPlatform != NULL) && (collisions.remove(p_OnPlatform) != 0)) {
		p_OnPlatform = NULL;
	}

	//
	if (getVelocity().getY() < 3) {
		if ((getVelocity().getY() <= 0) && ((getVelocity().getY() + 1.0f) > 0)) {
			df::Vector player_pos = getPosition();
			EventPlayerFalling* eventPlayerFalling = new EventPlayerFalling(player_pos);
			WM.onEvent(eventPlayerFalling);
		}
		//setVelocity(getVelocity() + *(new df::Vector(0, 2.0f)));
	}
	//Always falling down, but the maximum down velocity is 2
	else {
		setVelocity(df::Vector(getVelocity().getX(), 3));
	}

	drawHealthBar();
}

void Hero::jump() {
	setVelocity(df::Vector(0, -jumpHeight));
	setCentered(true);
	df::Vector player_pos = getPosition();
	EventPlayerJumping* eventPlayerJumping = new EventPlayerJumping(player_pos);
	WM.onEvent(eventPlayerJumping);
}

// Called with Saucer collides.
void Hero::hit(const df::EventCollision *p_collision_event) {
	std::string type1, type2;
	type1 = p_collision_event->getObject1()->getType();
	type2 = p_collision_event->getObject2()->getType();

	// If Hero on Hero, ignore.
	if ((type1 == "Hero") &&
		(type2 == "Hero"))
		return;
}

void Hero::takeDamage(df::Vector at, int damage) {
	health -= damage;
	new DamageIndicator(at, damage);
	DM.shake(damage, damage, 3, false);
	if (health <= 0) {
		WM.markForDelete(this);
		// Create GameOver object.
		GameOver *p_go = new GameOver(df::Vector(WM.getView().getCorner().getX(), 0) + df::Vector(WM.getView().getHorizontal() / 2, WM.getView().getVertical() / 2), false);
	}
}

void Hero::refillAmmo() {
	df::ObjectListIterator refillIterator(&weapon_list);
	refillIterator.first();
	while (!refillIterator.isDone()) {
		(dynamic_cast <Weapon*> (refillIterator.currentObject()))->refillAmmo();
		refillIterator.next();
	}
}

void Hero::drawHealthBar() {

	// Convert world position to view position.
	df::Vector window_pos = worldToView(getPosition() - df::Vector(getSprite()->getWidth() / 2, 0.5f + getSprite()->getHeight() / 2));

	// Convert spaces to pixels.
	df::Vector pixel_pos = spacesToPixels(window_pos);

	// Health bars are rectangles.
	static sf::RectangleShape shape;
	sf::Vector2f size(df::charWidth() * getSprite()->getWidth() * health / max_health, df::charHeight() / 3);
	shape.setSize(size);
	//shape.setPosition(pixel_pos.getX() - size.x / 2.0f + 5,
	//	pixel_pos.getY() - 8 * size.y / 1.75f - 5);
	shape.setPosition(pixel_pos.getX(), pixel_pos.getY());
	// Set color based on health.
	sf::Color color;
	float normalizedHealth = (float)health / (float)max_health;
	if (normalizedHealth > 0.7f)
	{
		color = sf::Color::Green;
	}
	else if (normalizedHealth > 0.3f)
	{
		color = sf::Color::Yellow;
	}
	else
	{
		color = sf::Color::Red;
	}
	shape.setFillColor(color);

	// Draw.
	DM.getWindow()->draw(shape);
}

Reticle *Hero::getReticle() {
	return p_reticle;
}
