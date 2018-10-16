
//Engine includes
#include "WorldManager.h"
#include "DisplayManager.h"
#include "EventStep.h"
#include "GameManager.h"

//Game code includes
#include "Level.h"
#include "Platform.h"
#include "Saucer.h"

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
			new Saucer(15, 5, 0);
		}

		return 1;
	}

	return 0;
}

void Level::draw() {
}