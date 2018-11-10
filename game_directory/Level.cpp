
//Engine includes
#include "WorldManager.h"
#include "DisplayManager.h"
#include "EventStep.h"
#include "GameManager.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "ObjectList.h"
#include "EventMouse.h"

//Game code includes
#include "Level.h"
#include "Platform.h"
#include "Saucer.h"
#include "AmmoRefill.h"
#include "Boss.h"

Level::Level() {
	setType("Level");
	df::Sprite *p_temp_sprite = RM.getSprite("level_intro");
	if (!p_temp_sprite)
		LM.writeLog("GameStart::GameStart(): Warning! Sprite 'level intro' not found");
	else {
		setSprite(p_temp_sprite);
		//setSpriteSlowdown(15);
	}
	registerInterest(df::STEP_EVENT);
	registerInterest(df::MSE_EVENT);
	started = false;
}

void Level::start() {
	hero = new Hero;
	progress = 0;
	started = true;
	unregisterInterest(df::MSE_EVENT);

	//printf("%f", WM.setViewFollowing((df::Object*)hero));

	// Create platforms to stand on
	Platform *platform = new Platform(df::Vector(7, 65));
	new Platform(df::Vector(70, 55));
	new Platform(df::Vector(133, 60));
	new Platform(df::Vector(196, 47));
	new Platform(df::Vector(263, 65));
	new Platform(df::Vector(317, 60));
	new Platform(df::Vector(371, 55));
	new Platform(df::Vector(425, 50));
	new Platform(df::Vector(479, 50));
	new Platform(df::Vector(533, 50));
	new Platform(df::Vector(593, 62));
	new Platform(df::Vector(593, 47));
	new Platform(df::Vector(593, 32));

	bossSpawned = false;

	// Create an ammo refill box
	AmmoRefill* ammo_refill = new AmmoRefill();
	ammo_refill->setPosition(df::Vector(196, 47) - df::Vector(20, (platform->getSprite()->getHeight() / 2) + (ammo_refill->getSprite()->getHeight() / 2)));

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
		if (started) {
			df::ObjectList object_list = WM.objectsOfType("Hero");
			if (object_list.getCount() > 0) {
				WM.setViewPosition(hero->getPosition() + df::Vector(WM.getView().getHorizontal() / 3, 0));
				if (hero->getPosition().getY() > WM.getView().getVertical()) {
					WM.markForDelete(hero);
				}
				else {
					float x = hero->getPosition().getX();
					if ((0 <= x) && (x <= 263)) {
						if (progress < 1) progress = 1;
					}
					else if ((263 < x) && (x <= 533)) {
						if (progress < 2) progress = 2;
					}
					else {
						if (progress < 3) progress = 3;
					}
				}
			}

			if (progress == 1) {
				if ((GM.getStepCount() % 180 == 0) ||
					(GM.getStepCount() % 180 == 5) ||
					(GM.getStepCount() % 180 == 10) ||
					(GM.getStepCount() % 180 == 15) ||
					(GM.getStepCount() % 180 == 20)) {
					Saucer *saucer = new Saucer(15, 5, 0, 3);
					df::Vector spawnPos(WM.getView().getCorner().getX() + WM.getView().getHorizontal(), 30);
					saucer->setPosition(spawnPos);
					saucer->markStart();
				}
			}
			else if (progress == 2) {
				int i = GM.getStepCount() % 180;
				df::Vector spawnPos(WM.getView().getCorner().getX() + WM.getView().getHorizontal(), 30);

				if ((i == 18) ||
					(i == 23) ||
					(i == 28) ||
					(i == 33) ||
					(i == 38)) {
					Saucer *saucer = new Saucer(15, 5, 0, 1);
					spawnPos += df::Vector(0, (i - 18) * 3 / 5);
					saucer->setPosition(spawnPos);
					saucer->markStart();
				}
			}
			else if (progress == 3) {
				if (!bossSpawned) {
					Boss* boss = new Boss(1100);
					boss->setPosition(df::Vector(650, 35.0f));
					boss->createWeakPoint(df::Vector(-8.0f, -10.0f), 5, 500);
					boss->createWeakPoint(df::Vector(10.0f, -9.0f), 5, 500);
					bossSpawned = true;
				}
			}
		}

		return 1;
	}

	return 0;
}

void Level::draw() {
	if (!started) {
		df::Object::draw();
	}
}