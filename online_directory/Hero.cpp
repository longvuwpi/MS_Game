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
#include "EventKeyboardNetwork.h"
#include "EventMouseNetwork.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "ObjectListIterator.h"
#include "DisplayManager.h"
#include "NetworkManager.h"

// Game includes.
#include "Weapon.h"
//#include "Bullet.h"
//#include "BulletTrail.h"
//#include "EventNuke.h"
//#include "Explosion.h"
//#include "GameOver.h"
#include "Hero.h"
#include "Client.h"

//#include "EventPlayerFalling.h"
//#include "EventPlayerJumping.h"
//#include "Saucer.h"
//#include "DamageIndicator.h"
#include "AmmoDisplay.h"

float moveSpeed = 2.0f;
float jumpHeight = 6.0f;
float down_gravity = 2.0f;

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
	registerInterest(df::NETWORK_KEYBOARD_EVENT);
	registerInterest(df::NETWORK_MSE_EVENT);

	// Need to update rate control each step.
	registerInterest(df::STEP_EVENT);

	// Set object type.
	setType("Hero");

	// Set starting location.
	//df::Vector p(7, WM.getBoundary().getVertical()/2);
	df::Vector p(70, 40);
	setPosition(p);

	//p_reticle->draw();

	// Set attributes that control actions.
	fire_slowdown = 5;
	fire_countdown = fire_slowdown;
	nuke_count = 1;
	jump_max = 4;
	jump_count = jump_max;
	max_health = 50;
	health = 50;
	isDucking = false;
	p_OnPlatform = NULL;

	if (NM.isServer()) {
		setAcceleration(df::Vector(0, down_gravity));

	}
	setSolidness(df::SOFT);

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

	////Weapon view
	//weapon_view = new df::ViewObject; // Count of nukes.
	//weapon_view->setLocation(df::TOP_LEFT);
	//weapon_view->setViewString(getCurrentWeapon()->getWeaponName() + ":");
	//weapon_view->setColor(df::YELLOW);
	//weapon_view->setSolidness(df::SPECTRAL);

	//// Create reticle for firing bullets and aiming.
	//
	if (!NM.isServer()) {
		new AmmoDisplay(this);
		p_reticle = new Reticle(this);
	}
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
			//Explosion *p_explosion = new Explosion("explosion", 0);
			//p_explosion->setPosition(temp_pos);
		}
	}

	// Mark Reticle for deletion.
	//WM.markForDelete(p_reticle);
}

// Handle event.
// Return 0 if ignored, else 1.
int Hero::eventHandler(const df::Event *p_e) {
	df::ObjectList ol = WM.objectsOfType("Client");
	df::ObjectListIterator oli(&ol);
	Client *client = (Client *)oli.currentObject();
	oli.first();

	if ((p_e->getType() == df::COLLISION_EVENT) && (NM.isServer())) {
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

	if ((p_e->getType() == df::NETWORK_KEYBOARD_EVENT) && (NM.isServer())) {
		df::EventKeyboardNetwork *p_n = (df::EventKeyboardNetwork *) p_e;
		df::EventKeyboard e;
		e.setKeyboardAction(p_n->getKeyboardAction());
		e.setKey(p_n->getKey());
		kbd(&e);
		return 1;
	}

	if ((p_e->getType() == df::NETWORK_MSE_EVENT) && (NM.isServer())) {
		df::EventMouseNetwork *p_n = (df::EventMouseNetwork *) p_e;
		df::EventMouse e;
		e.setMouseAction(p_n->getMouseAction());
		e.setMouseButton(p_n->getMouseButton());
		e.setMousePosition(p_n->getMousePosition());
		mouse(&e);

		return 1;
	}

	if ((p_e->getType() == df::STEP_EVENT) && (NM.isServer())) {
		step();
		return 1;
	}

	// If get here, have ignored this event.
	return 0;
}

void Hero::landedOn(Platform *platform) {
	//if (p_OnPlatform == NULL) {
	//	p_OnPlatform = platform;

	//	std::cout << "Landed on platform, hero pos: (" << getPosition().getX() << "," << getPosition().getY() << ")\n";
	//	//if player is hovering above platform, set player to be exactly on platform
	//	df::Vector delta(0, -0.5 + (p_OnPlatform->getPosition().getY() - getPosition().getY()) - ((p_OnPlatform->getSprite()->getHeight() + getSprite()->getHeight()) / 2));
	//	if (delta.getY() > 0)
	//		setPosition(getPosition() + delta);

	//	jump_count = jump_max;

	//	setCentered(true);

	//	return;
	//}
	if ((p_OnPlatform != platform) && (getVelocity().getY() >= 0)) {
		df::Vector delta(0, (platform->getPosition().getY() - getPosition().getY()) - ((platform->getSprite()->getHeight() + getSprite()->getHeight()) / 2));
		if ((-2 <= delta.getY()) && (delta.getY() <= 0.5)) {
			df::Vector newPosition(getPosition().getX(), platform->getPosition().getY() - (float)((platform->getSprite()->getHeight() + getSprite()->getHeight()) / 2));
			std::cout << "Putting hero on the platform from " << (float)getPosition().getY() << "to " << (float)newPosition.getY() << "\n";

			setPosition(newPosition);

			p_OnPlatform = platform;

			jump_count = jump_max;

			setAcceleration(df::Vector(0, 0));
			setVelocity(df::Vector(getVelocity().getX(), 0));

			setCentered(true);

			return;
		}
	}
}

// Take appropriate action according to mouse action.
void Hero::mouse(const df::EventMouse *p_mouse_event) {
	// Pressed button?
	if (((p_mouse_event->getMouseAction() == df::CLICKED) || (p_mouse_event->getMouseAction() == df::PRESSED)) &&
		(p_mouse_event->getMouseButton() == df::Mouse::LEFT)) {
		fire(p_mouse_event->getMousePosition());
	}
	else if ((p_mouse_event->getMouseAction() == df::CLICKED) &&
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
		if (p_keyboard_event->getKeyboardAction() == df::KEY_DOWN)
		{
			jump();
		}
		else if (p_keyboard_event->getKeyboardAction() == df::KEY_RELEASED) {
			if (p_OnPlatform == NULL) {
				jump_count = 0;
			}
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
			hero_modified[CURRENT_WEAPON] = true;
			getCurrentWeapon()->setVisible(true);
			//weapon_view->setViewString(getCurrentWeapon()->getWeaponName() + ":");
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
	//weapon_view->setViewString(getCurrentWeapon()->getWeaponName() + ":" + std::to_string(getCurrentWeapon()->getAmmoLoaded()) + "/" + std::to_string(getCurrentWeapon()->getAmmoBackup()));

	//Check to see if the hero is on a platform.
	df::ObjectList collisions = WM.isCollision(this, getPosition() + df::Vector(0, 0.5));
	if ((p_OnPlatform != NULL) && (collisions.remove(p_OnPlatform) != 0)) {
		p_OnPlatform = NULL;
	}

	////
	//if (getVelocity().getY() < 3) {
	//	if ((getVelocity().getY() <= 0) && ((getVelocity().getY() + 1.0f) > 0)) {
	//		df::Vector player_pos = getPosition();
	//		EventPlayerFalling* eventPlayerFalling = new EventPlayerFalling(player_pos);
	//		WM.onEvent(eventPlayerFalling);
	//	}
	//	//setVelocity(getVelocity() + *(new df::Vector(0, 2.0f)));
	//}
	////Always falling down, but the maximum down velocity is 2
	//else {  
	//	setVelocity(df::Vector(getVelocity().getX(), 3));
	//}

	if (p_OnPlatform == NULL) {
		setAcceleration(df::Vector(0, down_gravity));
	}

	if (getVelocity().getY() >= 3) {
		setVelocity(df::Vector(getVelocity().getX(), 3));
	}

	drawHealthBar();
}

void Hero::jump() {
	if (jump_count > 0) {
		p_OnPlatform = NULL;

		//setPosition(getPosition() + df::Vector(0, -1));
		if (!isDucking) {
			setVelocity(df::Vector(0, -jumpHeight));
		}

		setCentered(true);
	
		jump_count--;
	
	}
	//df::Vector player_pos = getPosition();
	//EventPlayerJumping* eventPlayerJumping = new EventPlayerJumping(player_pos);
	//WM.onEvent(eventPlayerJumping);
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
	//new DamageIndicator(at, damage);
	DM.shake(damage*2, damage*2, 3, false);

	df::ObjectList go_objects = WM.objectsOfType("GameOver");
	if (go_objects.getCount() <= 0) {
		if (health <= 0) {
			WM.markForDelete(this);
			// Create GameOver object.
			//GameOver *p_go = new GameOver(df::Vector(WM.getView().getCorner().getX(), 0) + df::Vector(WM.getView().getHorizontal() / 2, WM.getView().getVertical() / 2), false);
		}
	}
}

void Hero::refillAmmo() {
	//df::ObjectListIterator refillIterator(&weapon_list);
	//refillIterator.first();
	//while (!refillIterator.isDone()) {
	//	(dynamic_cast <Weapon*> (refillIterator.currentObject()))->refillAmmo();
	//	refillIterator.next();
	//}
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

//Reticle *Hero::getReticle() {
//	return p_reticle;
//}

bool Hero::isModified() {
	for (int i = 0; i < hero_att_count; i++) {
		if (hero_modified[i]) return true;
	}
	return Object::isModified();
}

std::string Hero::serialize(bool all) {
	LM.writeLog(5, "Hero::serialize()");

	// Do main serialize from parent.
	std::string s = Object::serialize(all);

	// Add Weapon-specific attributes.
	if (all || hero_modified[CURRENT_WEAPON]) {
		s += "current_weapon:" + df::toString(getCurrentWeapon()->getWeaponType()) + ",";
		hero_modified[CURRENT_WEAPON] = false;
	}

	return s;
}

int Hero::deserialize(std::string str) {
	LM.writeLog("Hero::deserialize()");

	// Do main deserialize from parent.
	Object::deserialize(str);

	// Get ready for parsing.
	std::string val;

	val = df::match("", "current_weapon");
	if (!val.empty()) {
		int i = atoi(val.c_str());
		LM.writeLog("Hero::deserialize(): current weapon is %d", i);
		while (getCurrentWeapon()->getWeaponType() != i) {
			weapon_selector->next();
			if (weapon_selector->isDone()) {
				weapon_selector->first();
			}
		}
	}

	return 0;
}