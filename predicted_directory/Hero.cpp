//
// Hero.cpp
//
#include <iostream>
#include <stdio.h>

// Engine includes.
#include "EventCollision.h"
#include "EventMouse.h"
#include "EventKeyboard.h"
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
#include "Server.h"
//#include "EventPlayerFalling.h"
//#include "EventPlayerJumping.h"
//#include "Saucer.h"
//#include "DamageIndicator.h"
#include "AmmoDisplay.h"

float moveSpeed = 2.0f;
float jumpHeight = 6.0f;
float down_gravity = 2.0f;

Hero::Hero(int client_hero_id, int hero_id, bool isPredicted) {
	is_predicted = isPredicted;
	hero_modified[IS_PREDICTED] = true;

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
	df::Vector p(60, 15);
	setPosition(p);
	//if (is_predicted) m_modified[df::POSITION] = false;

	//p_reticle->draw();

	// Set attributes that control actions.
	fire_slowdown = 5;
	fire_countdown = fire_slowdown;
	nuke_count = 1;
	jump_max = 4;
	jump_count = jump_max;
	max_health = 50;
	health = 50;
	death_count = 0;
	isDucking = false;
	p_OnPlatform = NULL;

	//if (NM.isServer()) {
	setAcceleration(df::Vector(0, down_gravity));
	//if (is_predicted) m_modified[df::ACCELERATION] = false;

	//}
	setSolidness(df::SOFT);

	//int hero_id = (socket_index + 1) * 20;
	setId(hero_id);

	//Set up weapons
	Weapon *ak47 = new Weapon("AK47", WeaponType::RIFLE, this, 10, 3, 30, 90, 8, false, 0, 0, 2.5f);
	weapon_list.insert(ak47);

	Weapon *awp = new Weapon("AWP", WeaponType::SNIPER, this, 18, 40, 10, 30, 30, false, 0, 0, 2.5f);
	weapon_list.insert(awp);
	awp->setVisible(false);

	Weapon *grenade_launcher = new Weapon("GrenadeLauncher", WeaponType::LAUNCHER, this, 8, 40, 1, 30, 12, true, 0.4f, 20, 1.5f);
	weapon_list.insert(grenade_launcher);
	grenade_launcher->setVisible(false);

	weapon_selector = new df::ObjectListIterator(&weapon_list);
	weapon_selector->first();

	ak47->setId(hero_id + 1);
	awp->setId(hero_id + 2);
	grenade_launcher->setId(hero_id + 3);

	// setup world and view boundaries
	int world_horiz, world_vert;
	world_horiz = DM.getHorizontal() * 10;
	world_vert = DM.getVertical();
	int window_horiz = DM.getHorizontal();
	df::Vector world_corner(0, 0);
	df::Box boundary(world_corner, (float)world_horiz, (float)world_vert);
	WM.setBoundary(boundary);
	df::Box window_boundary(world_corner, (float)window_horiz, (float)world_vert);
	WM.setView(window_boundary);

	//// Create reticle for firing bullets and aiming.
	//
	is_main_hero = false;

	if (!NM.isServer()) {
		if (client_hero_id == hero_id) {
			new AmmoDisplay(this);
			p_reticle = new Reticle(this);
			is_main_hero = true;
		}
	}
}

bool Hero::isPredicted() {
	return is_predicted;
}

bool Hero::isMainHero() {
	return is_main_hero;
}

void Hero::setWalkingSprite() {
	setSprite(walk_sprite);
	setSpriteSlowdown(5);  // 1/3 speed animation.
	setTransparency();	   // Transparent sprite.
	setPosition(getPosition() + df::Vector(0, -0.5 + (duck_sprite->getHeight() / 2) - (walk_sprite->getHeight() / 2)));
	setCentered(true);
	//if (is_predicted) {
	//	m_modified[df::SPRITE] = false;
	//	m_modified[df::POSITION] = false;
	//}
}

void Hero::setDuckingSprite() {
	setSprite(duck_sprite);
	setPosition(getPosition() + df::Vector(0, -0.5 + (walk_sprite->getHeight() / 2) - (duck_sprite->getHeight() / 2)));
	setCentered(true);
	//if (is_predicted) {
	//	m_modified[df::SPRITE] = false;
	//	m_modified[df::POSITION] = false;
	//}
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

	if (NM.isServer()) {
		LM.writeLog("Hero with id %d died %d times", getId(), death_count);

		df::ObjectList ol = WM.objectsOfType("Server");
		df::ObjectListIterator oli(&ol);
		oli.first();
		Server *server = (Server *)oli.currentObject();
		server->sendMessage(df::DELETE_OBJECT, this);
	}

	// Mark Reticle for deletion.
	//if (is_main_hero)
	//WM.markForDelete(p_reticle);
}

// Handle event.
// Return 0 if ignored, else 1.
int Hero::eventHandler(const df::Event *p_e) {

	if ((p_e->getType() == df::COLLISION_EVENT) && (NM.isServer() || (is_predicted && is_main_hero))) {
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
		//hit(p_collision_event);
		return 1;
	}

	if ((p_e->getType() == df::NETWORK_KEYBOARD_EVENT) && (NM.isServer())) {
		df::EventKeyboardNetwork *p_n = (df::EventKeyboardNetwork *) p_e;
		if ((p_n->getSocketIndex() + 1) * 20 == getId()) {
			df::EventKeyboard e;
			e.setKeyboardAction(p_n->getKeyboardAction());
			e.setKey(p_n->getKey());
			kbd(&e);
			return 1;
		}
	}

	if ((p_e->getType() == df::NETWORK_MSE_EVENT) && (NM.isServer())) {
		df::EventMouseNetwork *p_n = (df::EventMouseNetwork *) p_e;
		if ((p_n->getSocketIndex() + 1) * 20 == getId()) {
			df::EventMouse e;
			e.setMouseAction(p_n->getMouseAction());
			e.setMouseButton(p_n->getMouseButton());
			e.setMousePosition(p_n->getMousePosition());
			mouse(&e);

			return 1;
		}
	}

	if (p_e->getType() == df::STEP_EVENT) {
		step();
		return 1;
	}

	if ((p_e->getType() == df::KEYBOARD_EVENT) && (!NM.isServer())) {
		LM.writeLog("Predicting keyboard input");
		kbd((df::EventKeyboard *)p_e);
	}

	// If get here, have ignored this event.
	return 0;
}

void Hero::landedOn(Platform *platform) {
	//if (!NM.isServer()) {
	//	LM.writeLog("Hero collided with platform");
	//}
	if (platform != NULL) {
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

				//clear all flags since code is executed similarly on server and clients
				//if (is_predicted) {
				//	m_modified[df::POSITION] = false;
				//	m_modified[df::ACCELERATION] = false;
				//	m_modified[df::SPEED] = false;
				//	m_modified[df::DIRECTION] = false;
				//	m_modified[df::SPRITE] = false;
				//}
				return;
			}
		}
	}
}

// Take appropriate action according to mouse action.
void Hero::mouse(const df::EventMouse *p_mouse_event) {
	// Pressed button?
	if (((p_mouse_event->getMouseAction() == df::CLICKED) || (p_mouse_event->getMouseAction() == df::PRESSED)) &&
		(p_mouse_event->getMouseButton() == df::Mouse::LEFT)) {
		fire(viewPositionOnHero() + (p_mouse_event->getMousePosition()));
		m_modified[df::POSITION] = true;
		m_modified[df::SPEED] = true;
		m_modified[df::ACCELERATION] = true;
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
		if ((p_keyboard_event->getKeyboardAction() == df::KEY_PRESSED) && (NM.isServer())) {
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
	{
		world_manager.moveObject(this, new_pos);
		//if (is_predicted) m_modified[df::POSITION] = false;
	}
}

Weapon *Hero::getCurrentWeapon() {
	if (weapon_selector->currentObject() != NULL) {
		return (dynamic_cast <Weapon*> (weapon_selector->currentObject()));
	}
	else return NULL;
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

	if (NM.isServer() || (is_predicted && is_main_hero)) {
		//Check to see if the hero is on a platform.
		//df::ObjectList collisions = WM.isCollision(this, getPosition() + df::Vector(0, 0.5));
		df::ObjectList collisions = WM.isCollision(this, getPosition());
		if ((p_OnPlatform != NULL) && (collisions.remove(p_OnPlatform) != 0)) {
			p_OnPlatform = NULL;
		}

		if (p_OnPlatform == NULL) {
			setAcceleration(df::Vector(0, down_gravity));
			//if (is_predicted) m_modified[df::ACCELERATION] = false;
		}

		if (getVelocity().getY() >= 3) {
			setVelocity(df::Vector(getVelocity().getX(), 3));
			//if (is_predicted) {
			//	m_modified[df::SPEED] = false;
			//	m_modified[df::DIRECTION] = false;
			//}
		}

		if (NM.isServer()) {
			if (getPosition().getY() > WM.getView().getVertical()) {
				//df::ObjectList go_objects = WM.objectsOfType("GameOver");
				//if (go_objects.getCount() <= 0) {
				takeDamage(getPosition(), 100);
				//}
			}
		}


		getCurrentWeapon()->setAcceleration(getAcceleration());
		getCurrentWeapon()->setVelocity(getVelocity());
		//if (is_predicted) {
		//	m_modified[df::ACCELERATION] = false;
		//	m_modified[df::SPEED] = false;
		//	m_modified[df::DIRECTION] = false;
		//}
	}

	drawHealthBar();
}

void Hero::jump() {
	if (jump_count > 0) {
		p_OnPlatform = NULL;

		//setPosition(getPosition() + df::Vector(0, -1));
		if (!isDucking) {
			setVelocity(df::Vector(0, -jumpHeight));
			//if (is_predicted) {
			//	m_modified[df::SPEED] = false;
			//	m_modified[df::DIRECTION] = false;
			//}
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
	hero_modified[HEALTH] = true;
	m_modified[df::POSITION] = true;
	m_modified[df::SPEED] = true;
	m_modified[df::ACCELERATION] = true;

	//LM.writeLog("Hero took damage!! It hurts! Current health is %d", health);
	//new DamageIndicator(at, damage);
	DM.shake(damage * 2, damage * 2, 3, false);

	df::ObjectList go_objects = WM.objectsOfType("GameOver");
	if (go_objects.getCount() <= 0) {
		if (health <= 0) {
			//WM.markForDelete(this);
			//// Create GameOver object.
			////GameOver *p_go = new GameOver(df::Vector(WM.getView().getCorner().getX(), 0) + df::Vector(WM.getView().getHorizontal() / 2, WM.getView().getVertical() / 2), false);
			df::Vector p(60, 15);
			p_OnPlatform = NULL;
			setPosition(p);
			health = max_health;
			refillAmmo();
			death_count++;
			LM.writeLog("Hero with id %d died %d times", getId(), death_count);
		}
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
	df::Vector world_pos = getPosition() - df::Vector(getSprite()->getWidth() / 2, 0.5f + getSprite()->getHeight() / 2);

	// Convert world position to view position.
	df::Vector window_pos = worldToView(world_pos);

	// Convert spaces to pixels.
	df::Vector pixel_pos = spacesToPixels(window_pos);

	// Health bars are rectangles.
	static sf::RectangleShape shape;
	static sf::RectangleShape bgShape;

	//Set size
	sf::Vector2f size(df::charWidth() * getSprite()->getWidth() * health / max_health, df::charHeight() / 3);
	sf::Vector2f bgSize(df::charWidth() * getSprite()->getWidth(), df::charHeight() / 1.5f);
	bgShape.setSize(bgSize);
	shape.setSize(size);

	//Set position
	shape.setPosition(pixel_pos.getX(), pixel_pos.getY());
	bgShape.setPosition(pixel_pos.getX(), pixel_pos.getY());

	// Set color based on health.
	sf::Color color;
	sf::Color bgColor;
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
	bgColor = is_main_hero? sf::Color::Cyan : sf::Color::White;
	shape.setFillColor(color);
	bgShape.setFillColor(bgColor);

	// Draw.
	DM.getWindow()->draw(bgShape);
	DM.getWindow()->draw(shape);

	if (NM.isServer()) {
		DM.drawString(world_pos - df::Vector(0,2.5f), is_predicted ? "true" : "false", df::CENTER_JUSTIFIED, df::WHITE);
	}
	else {
		int platformid = 0;
		if (p_OnPlatform != NULL) platformid = p_OnPlatform->getId();
		DM.drawString(world_pos - df::Vector(0, 2.5f), std::to_string(platformid) , df::CENTER_JUSTIFIED, df::WHITE);
	}

}

Reticle *Hero::getReticle() {
	return p_reticle;
}

int Hero::getHealth() {
	return health;
}

bool Hero::isModified() const {
	for (int i = 0; i < hero_att_count; i++) {
		if (hero_modified[i]) return true;
	}
	return Object::isModified();
}

std::string Hero::serialize(std::string all) {
	LM.writeLog(1, "Hero::serialize() %s", all);

	// Do main serialize from parent.
	std::string s = Object::serialize(all);

	// Add Weapon-specific attributes.
	if ((all == "ALL") || hero_modified[HEALTH]) {
		LM.writeLog("Hero::serialize(): serializing health %d", health);
		s += "health:" + df::toString(getHealth()) + ",";
		hero_modified[HEALTH] = false;
	}

	if ((all == "ALL") || hero_modified[CURRENT_WEAPON]) {
		s += "current_weapon:" + df::toString(getCurrentWeapon()->getWeaponType()) + ",";
		hero_modified[CURRENT_WEAPON] = false;
	}

	if ((all == "ALL") || hero_modified[IS_PREDICTED]) {
		std::string predicted = is_predicted ? "1" : "0";
		s += "is_predicted:" + predicted + ",";
		hero_modified[IS_PREDICTED] = false;
	}

	return s;
}

int Hero::deserialize(std::string str) {
	//LM.writeLog("Hero::deserialize()");
	std::string val;
	df::match(str, "");
	bool mainDeserialized = false;
	df::Vector currentPosition = getPosition();

	//figure out if a hero position was serialized
	float f;
	bool is_position = true;
	df::Vector new_position;
	val = df::match("", "pos-x");
	if (!val.empty()) {
		f = (float)atof(val.c_str());
		new_position.setX(f);
	}
	else
		is_position = false;
	val = df::match("", "pos-y");
	if (!val.empty()) {
		f = (float)atof(val.c_str());
		new_position.setY(f);
	}
	else
		is_position = false;


	// Do main deserialize from parent.

	if (is_main_hero) {
		if (!is_predicted) {
			Object::deserialize(str);
			mainDeserialized = true;
		}
		else {
			if (is_position) {
				if ((currentPosition - new_position).getMagnitude() > 55.0f) {
					Object::deserialize(str);
					mainDeserialized = true;
					LM.writeLog("Fixing up hero position");
				}
			}
		}
	}
	else {
		Object::deserialize(str);
		mainDeserialized = true;
	}

	//df::Vector serializedPosition = getPosition();
	//setPosition(currentPosition);

	// Get ready for parsing.

	val = df::match("", "health");
	if (!val.empty()) {
		int i = atoi(val.c_str());
		LM.writeLog("Hero::deserialize(): current health is %d", i);
		if ((!mainDeserialized) && is_position) {
			if ((fabs(currentPosition.getX() - new_position.getX()) > 54.0f) ||
				(fabs(currentPosition.getY() - new_position.getY()) > 35.0f))
			{
				Object::deserialize(str);
			}
		}
		health = i;
	}

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

	val = df::match("", "is_predicted");
	if (!val.empty()) {
		int i = atoi(val.c_str());
		LM.writeLog("Hero::deserialize(): is_predicted is %d", i);
		is_predicted = (i == 1) ? true : false;
	}


	return 0;
}

df::Vector Hero::viewPositionOnHero() {
	float x, y;
	df::Vector view_pos = getPosition() + df::Vector(WM.getView().getHorizontal() / 3, 0);
	// Center view at position view_pos.
	x = view_pos.getX() - WM.getView().getHorizontal() / 2;
	y = view_pos.getY() - WM.getView().getVertical() / 2;

	// Make sure horizontal not out of world boundaries.
	if (x + WM.getView().getHorizontal() > WM.getBoundary().getHorizontal())
		x = WM.getBoundary().getHorizontal() - WM.getView().getHorizontal();
	if (x < 0)
		x = 0;

	// Make sure vertical not out of world boundaries.
	if (y + WM.getView().getVertical() > WM.getBoundary().getVertical())
		y = WM.getBoundary().getVertical() - WM.getView().getVertical();
	if (y < 0)
		y = 0;

	return df::Vector(x, y);
}

void Hero::setIsPredicted(bool newisPredicted) {
	is_predicted = newisPredicted;
	hero_modified[IS_PREDICTED] = true;
}