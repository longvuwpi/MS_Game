#include "GameManager.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "ResourceManager.h"

#include "Level1.h"
#include "Level.h"
#include "Saucer.h"
#include "Boss.h"
//#include "AmmoRefill.h"

Level1::Level1() : Level("level_1") {
}

void Level1::levelLogic() {
	df::ObjectList hero_list = WM.objectsOfType("Hero");
//df::ObjectListIterator i(&hero_list);
//i.first();
//hero = (Hero *) i.currentObject();
	int scale = hero_list.getCount();

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

	if (progress == 1) {
		int i = (GM.getStepCount() - start_frame) % 180;
		if ((i == 0) ||
			(i == 5) ||
			(i == 10) ||
			(i == 15) ||
			(i == 20)) {
		//if (i == 0) {
			Saucer *saucer = new Saucer(16*scale, 5, 0, 3, 1);
			//df::Vector spawnPos(WM.getView().getCorner().getX() + WM.getView().getHorizontal(), 30);
			df::Vector spawnPos(hero->viewPositionOnHero().getX() + WM.getView().getHorizontal(), 30);
			saucer->setPosition(spawnPos);
			saucer->markStart();
		}
	}
	else if (progress == 2) {
		int i = (GM.getStepCount() - start_frame) % 180;
		//df::Vector spawnPos(WM.getView().getCorner().getX() + WM.getView().getHorizontal(), 30);
		df::Vector spawnPos(hero->viewPositionOnHero().getX() + WM.getView().getHorizontal(), 30);

		if ((i == 18) ||
			(i == 23) ||
			(i == 28) ||
			(i == 33) ||
			(i == 38)) {
			Saucer *saucer = new Saucer(16*scale, 5, 0, 1, 2);
			spawnPos += df::Vector(0, (i - 18) * 3 / 5);
			saucer->setPosition(spawnPos);
			saucer->markStart();
		}
	}
	else if (progress == 3) {
		if (!bossSpawned) {
			RM.getMusic("music_theme")->stop();
			RM.getMusic("music_boss")->play();
			boss = new Boss(1100*scale);
			boss->setPosition(df::Vector(700, 35.0f));
			boss->createWeakPoint(df::Vector(-8.0f, -10.0f), 5, 500*scale);
			boss->createWeakPoint(df::Vector(10.0f, -9.0f), 5, 500*scale);
			bossSpawned = true;
			start_frame = GM.getStepCount();
		}
		else {
			df::ObjectList saucerlist = WM.objectsOfType("Saucer");
			//if (saucerlist.isEmpty()) {
			//	progress = 4;
			//	levelComplete();
			//}
			if (saucerlist.remove(boss) != 0) {
				df::ObjectList go_list = WM.objectsOfType("GameOver");
				if (go_list.isEmpty()) {
					progress = 4;
					levelComplete();
				}
			}
			else {
				int i = (GM.getStepCount() - start_frame) % 150;
				//df::Vector spawnPos(WM.getView().getCorner().getX() + WM.getView().getHorizontal(), 30);
				df::Vector spawnPos(hero->viewPositionOnHero().getX() + WM.getView().getHorizontal(), 30);

				if ((i == 18) ||
					(i == 23) ||
					(i == 28) ||
					(i == 33)) {
					Saucer *saucer = new Saucer(13*scale, 3, 0, 1, 1);
					spawnPos += df::Vector(0, (i - 18) * 3 / 5);
					saucer->setPosition(spawnPos);
					saucer->markStart();
				}
			}
		}
	}

}

void Level1::initialize() {
	// Create platforms to stand on
	new Platform(df::Vector(7, 65));
	new Platform(df::Vector(70, 55));
	new Platform(df::Vector(133, 60));
	new Platform(df::Vector(150, 27));
	new Platform(df::Vector(196, 47));
	new Platform(df::Vector(225, 30));
	new Platform(df::Vector(263, 65));
	new Platform(df::Vector(317, 60));
	new Platform(df::Vector(330, 40));
	new Platform(df::Vector(371, 55));
	new Platform(df::Vector(400, 37));
	new Platform(df::Vector(425, 50));
	new Platform(df::Vector(479, 50));
	new Platform(df::Vector(533, 50));
	new Platform(df::Vector(593, 62));
	new Platform(df::Vector(593, 47));
	new Platform(df::Vector(593, 32));

	bossSpawned = false;

	// Create an ammo refill box
	//AmmoRefill* ammo_refill = new AmmoRefill();
	//ammo_refill->setPosition(df::Vector(533, 50) - df::Vector(20, (platform->getSprite()->getHeight() / 2) + (ammo_refill->getSprite()->getHeight() / 2)));

	//// setup world and view boundaries
	//int world_horiz, world_vert;
	//world_horiz = DM.getHorizontal() * 10;
	//world_vert = DM.getVertical();
	//int window_horiz = DM.getHorizontal();
	//df::Vector world_corner(0, 0);
	//df::Box boundary(world_corner, (float)world_horiz, (float)world_vert);
	//WM.setBoundary(boundary);
	//df::Box window_boundary(world_corner, (float)window_horiz, (float)world_vert);
	//WM.setView(window_boundary);
}