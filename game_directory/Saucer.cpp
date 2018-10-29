//
// Saucer.cpp
//

// System includes.
#include <stdlib.h>		// for rand()
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <iostream>
#include <stdio.h>
// Engine includes.
#include "EventCollision.h"
#include "EventNuke.h"
#include "EventOut.h"
#include "EventView.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "utility.h"
#include "DisplayManager.h"
#include "Sprite.h"
#include "Frame.h"
#include "Config.h"
#include "ObjectListIterator.h"
#include "EventStep.h"
#include "GameManager.h"

// Game includes.
#include "Explosion.h"
#include "Points.h"
#include "Saucer.h"
#include "Bullet.h"
#include "BulletTrail.h"
#include "DamageIndicator.h"


Saucer::Saucer(int maxHealth, int dmg, float radius, int movementType) {
	enemy_type = EnemyType::MINION;
	max_health = maxHealth;
	health = max_health;
    damage = dmg;
    bullet_radius = radius;
	movement_type = movementType;

    fire_count_down = 30;
	// Setup "saucer" sprite.
	df::Sprite *p_temp_sprite = RM.getSprite("saucer");
    
	if (!p_temp_sprite)
		LM.writeLog("Saucer::Saucer(): Warning! Sprite '%s' not found", "saucer");
	else {
		setSprite(p_temp_sprite);
		setSpriteSlowdown(30);
	}

	setTransparency('#');

	// Set object type.
	setType("Saucer");
    bullet_sprite = RM.getSprite("AK47_bullet");

	// Move Saucer to start location.
	moveToStart();
	// Register interest in "nuke" event.
	registerInterest(NUKE_EVENT);
    registerInterest(df::STEP_EVENT);
    
}

Saucer::~Saucer() {
	// Send "view" event with points to interested ViewObjects.
	// Add 10 points.
	df::EventView ev(POINTS_STRING, 10, true);
	WM.onEvent(&ev);
}


// Handle event.
// Return 0 if ignored, else 1.
int Saucer::eventHandler(const df::Event *p_e) {

	if (p_e->getType() == df::OUT_EVENT) {
		out();
		return 1;
	}

	if (p_e->getType() == df::COLLISION_EVENT) {
		const df::EventCollision *p_collision_event = dynamic_cast <df::EventCollision const *> (p_e);
		hit(p_collision_event);
		return 1;
	}

	if (p_e->getType() == NUKE_EVENT) {
		const EventNuke *p_nuke_event = dynamic_cast <EventNuke const *> (p_e);
		if ((getPosition() - p_nuke_event->getOrigin()).getMagnitude() <= p_nuke_event->getRadius()) {
			// Create explosion.
			Explosion *p_explosion = new Explosion("explosion", 0);
			p_explosion->setPosition(this->getPosition());

			takeDamage(getPosition(), p_nuke_event->getDamage());

			// Saucers appear stay around perpetually
			//new Saucer;
		}
	}

    if (p_e->getType() == df::STEP_EVENT) {
        step();
        return 1;
    }

	// If get here, have ignored this event.
	return 0;
}

// If moved off left edge, move back to far right.
void Saucer::out() {

	// If haven't moved off left edge, then nothing to do.
	if ((getPosition().getY() >= 0) && (getPosition().getY() <= WM.getView().getVertical()))
		return;

	// Otherwise, move back to far right.
	moveToStart();
}

// Called with Saucer collides.
void Saucer::hit(const df::EventCollision *p_collision_event) {
	std::string type1, type2;
	type1 = p_collision_event->getObject1()->getType();
	type2 = p_collision_event->getObject2()->getType();

	// If Hero, mark both objects for destruction.
	if ((type1 == "Hero") ||
		(type2 == "Hero")) {
		die();
		if (type1 == "Hero") {
			dynamic_cast <Hero *> (p_collision_event->getObject1())->takeDamage(p_collision_event->getPosition(), health);
		}
		else {
			dynamic_cast <Hero *> (p_collision_event->getObject2())->takeDamage(p_collision_event->getPosition(), health);
		}
	}

}

// Move Saucer to starting location on right side of window.
void Saucer::moveToStart() {
	df::Vector temp_pos;
	df::Vector view_corner = WM.getView().getCorner();

	// Get world boundaries.
	int world_horiz = (int)WM.getView().getHorizontal();
	int world_vert = (int)WM.getView().getVertical();

	// x is off right side of window.
	//temp_pos.setX(world_horiz + rand() % (int)world_horiz);
	temp_pos.setX(view_corner.getX() + world_horiz - 10);
	// y is in vertical range.
	temp_pos.setY(rand() % (int)(world_vert - 4) + 4.0f);

	// If collision, move down slightly until empty space.
	df::ObjectList collision_list = WM.isCollision(this, temp_pos);
	while (!collision_list.isEmpty()) {
		temp_pos.setY(temp_pos.getY() + 1);
		collision_list = WM.isCollision(this, temp_pos);
	}

	WM.moveObject(this, temp_pos + (*(new df::Vector(WM.getView().getCorner().getX(), 0))));
	move_to_start_frame = GM.getStepCount();
	move_to_start_pos = getPosition();
}

void Saucer::fire(){
    //df::Vector origin = getPosition() + (df::Vector(getBox().getHorizontal() / 2, -1.5f));
    // See if time to fire.
	
    df::ObjectList object_list= WM.objectsOfType("Hero");
	if (object_list.getCount() > 0) {
		df::ObjectListIterator li(&object_list);
		li.first();
		df::Vector hero_pos = li.currentObject()->getPosition();

		std::cout << "Hero pos: (" << hero_pos.getX() << "," << hero_pos.getY() << ")\n";

		df::Vector origin = getPosition();

		// Fire Bullet towards target.
		// Compute normalized vector to position, then scale by speed (1).
		df::Vector v = hero_pos - origin;

		//df::Vector v = getDirection();
		v.normalize();
		v.scale(2);
		Bullet *p = new Bullet(origin, bullet_sprite, damage, bullet_radius);
		p->setVelocity(v);
	}
}

void Saucer::step() {
	if (health <= 0) {
		die();
		return;
	}

	//Only minions fire for now
	if (enemy_type == EnemyType::MINION) {
		// Fire countdown.
		fire_count_down--;
		if (fire_count_down <= 0)
		{
			fire_count_down = 90;
			fire();
		}
	}

	move();
}

void Saucer::move() {
	switch (movement_type) {
	case 1:
	{
		LM.writeLog("Saucer moving");
		float delta_x = sin(GM.getStepCount()*5*3.14159265 / 180)*15 - (GM.getStepCount() - move_to_start_frame)*(0.2);
		float delta_y = sin((90 - GM.getStepCount()*5)*3.14159265 / 180)*6;
		setPosition(move_to_start_pos + df::Vector(delta_x, delta_y));
		std::cout << "Saucer Pos: " << delta_x << "," << delta_y << "\n";
		break;
	}
	default:
		break;
	}

	return;
}
//void Saucer::draw() {
//	sf::RenderWindow *m_p_window = DM.getWindow();
//	sf::RectangleShape *m_p_rectangle = new sf::RectangleShape(sf::Vector2f(df::charWidth(),
//		df::charHeight()));
//	df::Frame current_frame = getSprite()->getFrame(getSpriteIndex());
//	std::string frame_string = current_frame.getString();
//	sf::Text *m_p_text = new sf::Text();
//	sf::Font *m_font = new sf::Font();
//	m_font->loadFromFile(df::Config::getInstance().getFontFile());
//	m_p_text->setFont(*m_font);
//	m_p_text->setStyle(sf::Text::Bold); // Make bold, since looks better.
//	if (df::charWidth() < df::charHeight())
//		m_p_text->setCharacterSize((unsigned int)(df::charWidth() *
//			df::Config::getInstance().getFontScale()));
//	else
//		m_p_text->setCharacterSize((unsigned int)(df::charHeight() *
//			df::Config::getInstance().getFontScale()));
//
//	// Make sure window is allocated.
//	for (int i = 0; i < getSprite()->getWidth(); i++) {
//		for (int j = 0; j < getSprite()->getHeight(); j++) {
//			df::Vector world_pos(getPosition() + getBox().getCorner() + df::Vector(i, j));
//			// Convert to world position to window position.
//			df::Vector window_pos = df::worldToView(world_pos);
//
//			// Convert spaces (x,y) to pixels (x,y).
//			df::Vector pixel_pos = df::spacesToPixels(window_pos);
//
//			char to_draw = frame_string[j * getSprite()->getWidth() + i];
//
//			if (to_draw == getTransparency()) {
//				m_p_rectangle->setFillColor(sf::Color::White);
//				// First, draw black rectangle since text is "see through" in SFML.
//				m_p_rectangle->setPosition(pixel_pos.getX() - df::charWidth() / 10,
//					pixel_pos.getY() + df::charHeight() / 5);
//				m_p_window->draw(*m_p_rectangle);
//			}
//			else {
//				m_p_rectangle->setFillColor(sf::Color::Black);
//				m_p_rectangle->setPosition(pixel_pos.getX() - df::charWidth() / 10,
//					pixel_pos.getY() + df::charHeight() / 5);
//				m_p_window->draw(*m_p_rectangle);
//
//				unsigned char r, g, b;
//				df::colorToRGB(getSprite()->getColor(), r, g, b);
//				m_p_text->setString(to_draw);
//
//				// Set position in window (in pixels).
//				m_p_text->setPosition(pixel_pos.getX(), pixel_pos.getY());
//
//				// Set color.
//				m_p_text->setColor(sf::Color(r, g, b));
//
//				// Draw character.
//				m_p_window->draw(*m_p_text);
//
//
//			}
//		}
//	}
//	free(m_p_text);
//	free(m_font);
//	free(m_p_rectangle);
//	DM.drawString(getPosition(), std::to_string(health), df::CENTER_JUSTIFIED, df::WHITE);
//
//	// Advance sprite index, if appropriate.
//	if (getSpriteSlowdown() != 0) { // a '0' means sprite animation stopped
//		if (getSpriteSlowdownCount() + 1 >= getSpriteSlowdown()) {
//			setSpriteSlowdownCount(0);
//			setSpriteIndex(getSpriteIndex() + 1);
//			if (getSpriteIndex() >= (getSprite()->getFrameCount()))
//				setSpriteIndex(0);
//		}
//		else
//			setSpriteSlowdownCount(getSpriteSlowdownCount() + 1);
//	}
//
//
//	return;
//}


int Saucer::getHealth() {
	return health;
}

void Saucer::takeDamage(df::Vector at, int damage) {
	health -= damage;
	new DamageIndicator(at, damage);
}

void Saucer::die() {
	//Explosion *p_explosion = new Explosion("explosion", 0);
	//p_explosion->setPosition(this->getPosition());
	//df::addParticles(df::ParticleType::SPARKS, getPosition(), 2.0, df::YELLOW);
	//df::addParticles(df::ParticleType::SPARKS, getPosition(), 1.0, df::RED);
	//df::addParticles(100, 10, getPosition(), 0.1f, df::Vector(0, 0), 0.0f, 5.0f, 4.0f, 0.5f, 1.0f, 7, 10, (unsigned char)255, (char)255, (unsigned char)250, (unsigned char)250, (unsigned char)200, (unsigned char)100);
	df::addParticles(100, 10, getPosition(), 0.1f, df::Vector(0, 1), 1.0f, 5.0f, 4.0f, 0.5f, 1.0f, 7, 10, (unsigned char)255, (char)255, (unsigned char)250, (unsigned char)0, (unsigned char)0, (unsigned char)100);
	
	//Play "explode" sound.
	df::Sound *p_sound = RM.getSound("explode");
	p_sound->play();

	WM.markForDelete(this);
}

EnemyType Saucer::getEnemyType() {
	std::cout << "Enemy has health " << health << "\n";
	return enemy_type;
}

void Saucer::setEnemyType(EnemyType enemyType) {
	enemy_type = enemyType;
}