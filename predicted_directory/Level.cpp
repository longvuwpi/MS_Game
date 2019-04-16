
//Engine includes
#include "WorldManager.h"
#include "DisplayManager.h"
#include "EventStep.h"
#include "GameManager.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "ObjectList.h"
#include "EventMouse.h"
#include "NetworkManager.h"

//Game code includes
#include "Level.h"
#include "Platform.h"
#include "Saucer.h"
//#include "AmmoRefill.h"
#include "Boss.h"
#include "GameOver.h"
#include "Server.h"

Level::Level(std::string level_name) {
	setType("Level");
	df::Sprite *p_temp_sprite = RM.getSprite(level_name);
	if (!p_temp_sprite)
		LM.writeLog("GameStart::GameStart(): Warning! Level name sprite not found");
	else {
		setSprite(p_temp_sprite);
		//setSpriteSlowdown(15);
	}
	registerInterest(df::STEP_EVENT);
	//registerInterest(df::MSE_EVENT);
	setSolidness(df::SPECTRAL);
	started = false;
	start_frame = 0;
	setId(100);
	df::Object::max_id = 100;
	LM.writeLog("\n\n*******\nNew level object created\n*******\n");
}

Level::Level() {
	setType("Level");
	setSolidness(df::SPECTRAL);
	setId(100);
	df::Object::max_id = 100;
	LM.writeLog("\n\n*******\nNew level object created\n*******\n");
}

Level::~Level() {
	if (NM.isServer()) {
		df::ObjectList ol = WM.objectsOfType("Server");
		df::ObjectListIterator oli(&ol);
		oli.first();
		Server *server = (Server *)oli.currentObject();
		server->sendMessage(df::DELETE_OBJECT, this);
	}
}

void Level::start() {
	df::ObjectList object_list = WM.getAllObjects(true);
	df::ObjectListIterator i(&object_list);
	for (i.first(); !i.isDone(); i.next()) {
		df::Object *p_o = i.currentObject();
		if (p_o->getType() == "Level" || p_o->getType() == "LevelSelection") {
			if (p_o != this) {
				p_o->setActive(false);
			}
		}
	}

	//df::ObjectList hero_list = WM.objectsOfType("Hero");
	//df::ObjectListIterator i(&hero_list);
	//i.first();
	//hero = (Hero *) i.currentObject();

	RM.getMusic("start music")->pause();
	RM.getMusic("music_theme")->play();
	progress = 0;
	started = true;
	//unregisterInterest(df::MSE_EVENT);
	start_frame = GM.getStepCount();
	initialize();
}

int Level::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::MSE_EVENT) {
		if (!started) {
			const df::EventMouse *p_mouse_event = dynamic_cast <const df::EventMouse *> (p_e);
			if (p_mouse_event->getMouseAction() == df::MOVED) {
				// Change location to new mouse position.
				df::ObjectList at_mouse = WM.objectsAtPosition(p_mouse_event->getMousePosition());
				if (at_mouse.remove(this) == 0) {
					getSprite()->setColor(df::GREEN);
				}
				else {
					getSprite()->setColor(df::YELLOW);
				}
				return 1;
			}
			else if (p_mouse_event->getMouseAction() == df::CLICKED) {
				if (getSprite()->getColor() == df::GREEN) {
					start();
				}
				return 1;
			}
		}
	}

	if (p_e->getType() == df::STEP_EVENT) {
		step();
		return 1;
	}

	return 0;
}

void Level::step() {
	if (started) {
		df::ObjectList object_list = WM.objectsOfType("Hero");
		if (object_list.getCount() > 0) {
			df::ObjectListIterator i(&object_list);
			i.first();
			hero = (Hero *) i.currentObject();
			if (object_list.getCount() > 1) {
				i.next();
				if (hero->getPosition().getX() < i.currentObject()->getPosition().getX()) hero = (Hero*)i.currentObject();
			}
			//Set the camera position
			//WM.setViewPosition(hero->getPosition() + df::Vector(WM.getView().getHorizontal() / 3, 0));

			//Check if hero fell off the platforms
			levelLogic();
		}
		else {
			hero = NULL;
		}
	}
}

void Level::levelLogic() {
}

void Level::initialize() {
}

void Level::levelComplete() {
	GameOver *p_go = new GameOver(df::Vector(WM.getView().getCorner().getX(), 0) + df::Vector(WM.getView().getHorizontal() / 2, WM.getView().getVertical() / 2), true);
}

void Level::draw() {
	if (!started) {
		df::Object::draw();
	}
}

df::Vector Level::getTrackedHeroPos() {
	if (hero != NULL) return hero->getPosition();
	else return df::Vector(0, 0);
}

df::Vector Level::getTrackedHeroViewPos() {
	if (hero != NULL) return hero->viewPositionOnHero();
	else return df::Vector(0, 0);
}