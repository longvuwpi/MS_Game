//
// Saucer.cpp
//

// System includes.
#include <stdlib.h>		// for rand()
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

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

// Game includes.
#include "Explosion.h"
#include "Points.h"
#include "Saucer.h"

Saucer::Saucer() {

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

	// Set speed in horizontal direction.
	setVelocity(df::Vector(-1, 0)); // 1 space left every 4 frames

	// Move Saucer to start location.
	moveToStart();
	// Register interest in "nuke" event.
	registerInterest(NUKE_EVENT);
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

			// Delete self.
			WM.markForDelete(this);

			// Saucers appear stay around perpetually
			//new Saucer;
		}
	}

	// If get here, have ignored this event.
	return 0;
}

// If moved off left edge, move back to far right.
void Saucer::out() {

	// If haven't moved off left edge, then nothing to do.
	if (getPosition().getX() >= 0)
		return;

	// Otherwise, move back to far right.
	moveToStart();

	// Spawn new Saucer to make game get harder.
	new Saucer;
}

// Called with Saucer collides.
void Saucer::hit(const df::EventCollision *p_collision_event) {

	// If Saucer on Saucer, ignore.
	if ((p_collision_event->getObject1()->getType() == "Saucer") &&
		(p_collision_event->getObject2()->getType() == "Saucer"))
		return;

	// If Bullet, create explosion and make new Saucer.
	if ((p_collision_event->getObject1()->getType() == "Bullet") ||
		(p_collision_event->getObject2()->getType() == "Bullet") || 
		(p_collision_event->getObject1()->getType() == "BulletTrail") ||
		(p_collision_event->getObject2()->getType() == "BulletTrail")) {

		// Create an explosion.
		Explosion *p_explosion = new Explosion("explosion", 0);
		p_explosion->setPosition(this->getPosition());

		// Play "explode" sound.
		df::Sound *p_sound = RM.getSound("explode");
		p_sound->play();

		// Saucers appear stay around perpetually.
		WM.markForDelete(this);
	}

	// If Hero, mark both objects for destruction.
	if (((p_collision_event->getObject1()->getType()) == "Hero") ||
		((p_collision_event->getObject2()->getType()) == "Hero")) {
		WM.markForDelete(p_collision_event->getObject1());
		WM.markForDelete(p_collision_event->getObject2());
	}

}

// Move Saucer to starting location on right side of window.
void Saucer::moveToStart() {
	df::Vector temp_pos;

	// Get world boundaries.
	int world_horiz = (int)WM.getView().getHorizontal();
	int world_vert = (int)WM.getView().getVertical();

	// x is off right side of window.
	temp_pos.setX(world_horiz + rand() % (int)world_horiz);

	// y is in vertical range.
	temp_pos.setY(rand() % (int)(world_vert - 4) + 4.0f);

	// If collision, move right slightly until empty space.
	df::ObjectList collision_list = WM.isCollision(this, temp_pos);
	while (!collision_list.isEmpty()) {
		temp_pos.setX(temp_pos.getX() + 1);
		collision_list = WM.isCollision(this, temp_pos);
	}


	WM.moveObject(this, temp_pos + (*(new df::Vector(WM.getView().getCorner().getX(), 0))));
	//WM.moveObject(this, temp_pos);
}

void Saucer::draw() {
	sf::RenderWindow *m_p_window = DM.getWindow();
	sf::RectangleShape *m_p_rectangle = new sf::RectangleShape(sf::Vector2f(df::charWidth(),
		df::charHeight()));
	df::Frame current_frame = getSprite()->getFrame(getSpriteIndex());
	std::string frame_string = current_frame.getString();
	sf::Text *m_p_text = new sf::Text();
	sf::Font *m_font = new sf::Font();
	m_font->loadFromFile(df::Config::getInstance().getFontFile());
	m_p_text->setFont(*m_font);
	m_p_text->setStyle(sf::Text::Bold); // Make bold, since looks better.
	if (df::charWidth() < df::charHeight())
		m_p_text->setCharacterSize((unsigned int)(df::charWidth() *
			df::Config::getInstance().getFontScale()));
	else
		m_p_text->setCharacterSize((unsigned int)(df::charHeight() *
			df::Config::getInstance().getFontScale()));

	// Make sure window is allocated.
	for (int i = 0; i < getSprite()->getWidth(); i++) {
		for (int j = 0; j < getSprite()->getHeight(); j++) {
			df::Vector world_pos(getPosition() + getBox().getCorner() + df::Vector(i, j));
			// Convert to world position to window position.
			df::Vector window_pos = df::worldToView(world_pos);

			// Convert spaces (x,y) to pixels (x,y).
			df::Vector pixel_pos = df::spacesToPixels(window_pos);

			char to_draw = frame_string[j * getSprite()->getWidth() + i];

			if (to_draw == getTransparency()) {
				m_p_rectangle->setFillColor(sf::Color::White);
				// First, draw black rectangle since text is "see through" in SFML.
				m_p_rectangle->setPosition(pixel_pos.getX() - df::charWidth() / 10,
					pixel_pos.getY() + df::charHeight() / 5);
				m_p_window->draw(*m_p_rectangle);
			}
			else {
				m_p_rectangle->setFillColor(sf::Color::Black);
				m_p_rectangle->setPosition(pixel_pos.getX() - df::charWidth() / 10,
					pixel_pos.getY() + df::charHeight() / 5);
				m_p_window->draw(*m_p_rectangle);

				unsigned char r, g, b;
				df::colorToRGB(getSprite()->getColor(), r, g, b);
				m_p_text->setString(to_draw);

				// Set position in window (in pixels).
				m_p_text->setPosition(pixel_pos.getX(), pixel_pos.getY());

				// Set color.
				m_p_text->setColor(sf::Color(r, g, b));

				// Draw character.
				m_p_window->draw(*m_p_text);


			}
		}
	}
	free(m_p_text);
	free(m_font);
	free(m_p_rectangle);

	// Advance sprite index, if appropriate.
	if (getSpriteSlowdown() != 0) { // a '0' means sprite animation stopped
		if (getSpriteSlowdownCount() + 1 >= getSpriteSlowdown()) {
			setSpriteSlowdownCount(0);
			setSpriteIndex(getSpriteIndex() + 1);
			if (getSpriteIndex() >= (getSprite()->getFrameCount()))
				setSpriteIndex(0);
		}
		else
			setSpriteSlowdownCount(getSpriteSlowdownCount() + 1);
	}

	return;

}