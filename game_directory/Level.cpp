
//Engine includes
#include "WorldManager.h"
#include "DisplayManager.h"
#include "EventStep.h"
#include "GameManager.h"

//Game code includes
#include "Level.h"
#include "Platform.h"
#include "Saucer.h"
#include "AmmoRefill.h"
#include "HealthPick.h"
#include "ItemPick.h"
#include "PowerPick.h"
#include "Boss.h"

Level::Level() {
	setType("Level");
	registerInterest(df::STEP_EVENT);
}

void Level::start() {
	hero = new Hero;

	//printf("%f", WM.setViewFollowing((df::Object*)hero));

	// Create platforms to stand on
	Platform* platform = new Platform(df::Vector(7, 65));
	Platform* platform2 = new Platform(df::Vector(70, 55));
	Platform* platform3 = new Platform(df::Vector(133, 60));
	Platform* platform4 = new Platform(df::Vector(196, 47));
	Platform* platform5 = new Platform(df::Vector(263, 65));

	// Create an ammo refill box
	ItemPick* ammo_pick = new ItemPick("AMMOREFILL", ItemType::AMMOREFILL);
	ammo_pick->setPosition(df::Vector(196, 47) - df::Vector(20, (platform->getSprite()->getHeight()/2) + (ammo_pick->getSprite()->getHeight() / 2)));

    // Create a health pick sprite
    ItemPick* health_pick = new ItemPick("HEALTHPICK", ItemType::HEALTHPICK);
    health_pick->setPosition(df::Vector(263, 65) - df::Vector(20, (platform->getSprite()->getHeight()/2) + (health_pick->getSprite()->getHeight() / 2)));

    // Create a power pick sprite
    ItemPick* power_pick = new ItemPick("POWERUP", ItemType::POWERUP);
    power_pick->setPosition(df::Vector(133, 60) - df::Vector(20, (platform->getSprite()->getHeight()/2) + (power_pick->getSprite()->getHeight() / 2)));


    /*AmmoRefill* ammo_refill = new AmmoRefill();
    ammo_refill->setPosition(df::Vector(196, 47) - df::Vector(20, (platform->getSprite()->getHeight()/2) + (ammo_refill->getSprite()->getHeight() / 2)));

    HealthPick* health_pick = new HealthPick();
    health_pick->setPosition(df::Vector(133, 60) - df::Vector(20, (platform->getSprite()->getHeight()/2) + (health_pick->getSprite()->getHeight() / 2)));

    PowerPick* power_pick = new PowerPick();
    power_pick->setPosition(df::Vector(70, 55) - df::Vector(20, (platform->getSprite()->getHeight()/2) + (power_pick->getSprite()->getHeight() / 2)));
    */
	//Boss* boss = new Boss(1100);
	//boss->setPosition(df::Vector(300, 35));
	//boss->createWeakPoint(df::Vector(-8, -10), 5, 500);
	//boss->createWeakPoint(df::Vector(10, -9), 5, 500);

       
    Saucer* saucer = new Saucer(15, 5, 0);
    saucer->setPosition(df::Vector(200, 35));
    //saucer->setPosition(df::Vector(100, 35));
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
	if (p_e->getType() == df::STEP_EVENT) {
		df::ObjectList object_list = WM.objectsOfType("Hero");
		if (object_list.getCount() > 0) {
			WM.setViewPosition(hero->getPosition() + df::Vector(WM.getView().getHorizontal() / 3, 0));
			if (hero->getPosition().getY() > WM.getView().getVertical()) {
				WM.markForDelete(hero);
			}
		}
		if (GM.getStepCount() % 60 == 0) {
			Saucer *saucer = new Saucer(15, 5, 0,1);
			saucer->setVelocity(df::Vector(-1, 0)); // 1 space left every 4 frames
		}

		return 1;
	}

	return 0;
}

void Level::draw() {
}