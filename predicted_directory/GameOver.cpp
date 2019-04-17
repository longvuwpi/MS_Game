//
// GameOver.cpp
//

// Engine includes.
#include "EventStep.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "NetworkManager.h"

// Game includes.
#include "GameOver.h"
#include "GameStart.h"
#include "Client.h"
#include "Server.h"
#include "Hero.h"
#include "ReadyButton.h"

GameOver::GameOver(df::Vector position, bool win) {

	setType("GameOver");
	df::Sprite *p_temp_sprite;
	// Link to "message" sprite.
	if (!win) {
		p_temp_sprite = RM.getSprite("gameover");
	}
	else {
		p_temp_sprite = RM.getSprite("level_won");
	}

	if (!p_temp_sprite)
		LM.writeLog("GameOver::GameOver(): Warning! Sprite 'gameover' not found");
	else {
		setSprite(p_temp_sprite);
		setSpriteSlowdown(5);
		setTransparency('#');    // Transparent character.
		time_to_live = p_temp_sprite->getFrameCount() * 10;
	}

	// Put in center of window.
	//setLocation(df::CENTER_CENTER);
	setPosition(position);

	// Register for step event.
	registerInterest(df::STEP_EVENT);

	RM.getMusic("music_theme")->stop();
	RM.getMusic("music_boss")->stop();
	// Play "game over" sound.
	//df::Sound *p_sound;
	//if (win) {
	//	p_sound = RM.getSound("level_won");
	//}
	//else {
	//	p_sound = RM.getSound("game over");
	//}
	//p_sound->play();
}


GameOver::GameOver() {
	RM.getMusic("music_theme")->stop();
	RM.getMusic("music_boss")->stop();
	setType("GameOver");
	registerInterest(df::STEP_EVENT);
}

// When done, game over so reset things for GameStart.
GameOver::~GameOver() {

	if (NM.isServer()) {
		// Remove Saucers and ViewObjects, re-activate GameStart.
		df::ObjectList object_list = WM.getAllObjects(true);
		df::ObjectListIterator i(&object_list);
		for (i.first(); !i.isDone(); i.next()) {
			df::Object *p_o = i.currentObject();
			if (p_o->getType() == "Saucer" || p_o->getType() == "ViewObject" ||
				p_o->getType() == "Bullet" || p_o->getType() == "DamageIndicator" ||
				p_o->getType() == "Explosion" || p_o->getType() == "Level" ||
				p_o->getType() == "Platform" || p_o->getType() == "Weapon" ||
				p_o->getType() == "AmmoRefill" || p_o->getType() == "Instruction" ||
				p_o->getType() == "AmmoDisplay" || p_o->getType() == "Hero" ||
				p_o->getType() == "Reticle" ||
				p_o->getType() == "BulletTrail")
				WM.markForDelete(p_o);
			if (p_o->getType() == "ReadyButton") {
				((ReadyButton *)p_o)->showButton();
			}
		}

		df::ObjectList ol = WM.objectsOfType("Server");
		df::ObjectListIterator oli(&ol);
		oli.first();
		Server *server = (Server *)oli.currentObject();
		server->sendMessage(df::DELETE_OBJECT, this);
		server->introScene();
	}
	else {
		df::ObjectList object_list = WM.getAllObjects(true);
		df::ObjectListIterator i(&object_list);
		for (i.first(); !i.isDone(); i.next()) {
			df::Object *p_o = i.currentObject();
			if (p_o->getType() == "AmmoDisplay" || p_o->getType() == "Bullet" ||
				p_o->getType() == "Reticle" || p_o->getType() == "BulletTrail")
				WM.markForDelete(p_o);
		}
	}

}

// Handle event.
// Return 0 if ignored, else 1.
int GameOver::eventHandler(const df::Event *p_e) {

	if (p_e->getType() == df::STEP_EVENT) {
		step();
		return 1;
	}

	// If get here, have ignored this event.
	return 0;
}

// Count down to end of message.
void GameOver::step() {
	//setPosition(df::Vector(WM.getView().getCorner().getX(), 0) + df::Vector(WM.getView().getHorizontal() / 2, WM.getView().getVertical() / 2));
	if (NM.isServer()) {
		time_to_live--;
		if (time_to_live <= 0)
			WM.markForDelete(this);
	}
	else {
		setPosition(df::Vector(WM.getView().getCorner().getX(), 0) + df::Vector(WM.getView().getHorizontal() / 2, WM.getView().getVertical() / 2));
	}
}

// Override default draw so as not to display "value".
void GameOver::draw() {
	df::Object::draw();
}

int GameOver::deserialize(std::string str) {
	Object::deserialize(str);
	
	if (getSprite()->getLabel() == "gameover") {
		RM.getSound("game over")->play();
	}
	else {
		RM.getSound("level_won")->play();
	}

	return 0;
}
