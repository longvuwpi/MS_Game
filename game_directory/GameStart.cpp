//
// GameStart.cpp
//

// Engine includes.
#include "Color.h"
#include "EventKeyboard.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "Vector.h"
#include "EventStep.h"

// Game includes.
#include "GameStart.h"
#include "Hero.h"
#include "Music.h"
#include "Points.h"
#include "Saucer.h"
#include "Platform.h"
#include "Level.h"
#include "LevelSelection.h"

GameStart::GameStart() {
	setType("GameStart");

	// Link to "message" sprite.
	df::Sprite *p_temp_sprite = RM.getSprite("gamestart");
	if (!p_temp_sprite)
		LM.writeLog("GameStart::GameStart(): Warning! Sprite 'gamestart' not found");
	else {
		setTransparency('#');
		setSprite(p_temp_sprite);
		setSpriteSlowdown(15);
	}
	
	// Put in center of screen.
	setLocation(df::CENTER_CENTER);
	setCentered(true);

	// Register for "keyboard" event.
	registerInterest(df::KEYBOARD_EVENT);
	registerInterest(df::STEP_EVENT);

	// Play start music.
	p_music = RM.getMusic("start music");
	playMusic();
}

// Play start music.
void GameStart::playMusic() {
	p_music->play();
}

// Handle event.
// Return 0 if ignored, else 1.
int GameStart::eventHandler(const df::Event *p_e) {

	if (p_e->getType() == df::KEYBOARD_EVENT) {
		df::EventKeyboard *p_keyboard_event = (df::EventKeyboard *) p_e;
		switch (p_keyboard_event->getKey()) {
		case df::Keyboard::P: 			// play
			start();
			break;
		case df::Keyboard::Q:			// quit
			GM.setGameOver();
			break;
		default:
			break;
		}
		return 1;
	}

	if (p_e->getType() == df::STEP_EVENT) {
		df::Vector center = getPosition();
		float width = getBox().getHorizontal();
		float height = getBox().getVertical();
		float width_unit = width / floor(width);
		float height_unit = height / floor(height);

		df::Vector corner = center - df::Vector(width / 2, height / 2);

		if (GM.getStepCount() % 1 == 0) {
			for (int i = 0; i <= floor(width); i++) {
				for (int j = 0; j <= floor(height); j++) {
					if ((i == 0) || (i == floor(width)) || (j == 0) || (j == floor(height))) {
						float x = ((float)i) * width_unit;
						float y = ((float)j) * height_unit;
						df::Vector at = corner + df::Vector(x, y);
						df::Vector direction = at - center;
						direction.normalize();
						df::addParticles(5, 5, at, 0.6f, direction, 0.0f, 3.0f, 1.0f, 1.5f, 0.0f, 5, 3, (unsigned char)255, (char)255, (unsigned char)255, (unsigned char)255, (unsigned char)204, (unsigned char)255, df::ParticleClass::PARTICLE);
						//df::addParticles(5, 5, at, 2.0, direction, 0.2f, 3.0f, 5.0f, 1.5f, 0.0f, 20, 3, (unsigned char)255, (char)255, (unsigned char)255, (unsigned char)255, (unsigned char)204, (unsigned char)255, df::ParticleClass::PARTICLE);

						//df::addParticles(3, 5, at, 0.6f, direction, 3.0f, 1.0f, 1.0f, 1.0f, 1.0f, 5, 3, (unsigned char)255, (char)255, (unsigned char)255, (unsigned char)255, (unsigned char)153, (unsigned char)255, df::ParticleClass::PARTICLE);
				}
				}
			}
		}
	}
	// If get here, have ignored this event.
	return 0;
}

void GameStart::start() {
	//Level *testLevel = new Level();

	//// Setup heads-up display.
	//new Points;		                     // Points display.

	//// Pause start music.
	//p_music->pause();

	//testLevel->start();

	new LevelSelection();
	// When game starts, become inactive.
	setActive(false);
}

// Override default draw so as not to display "value".
void GameStart::draw() {
	df::Object::draw();
}
