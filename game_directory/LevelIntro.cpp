#include "WorldManager.h"
#include "DisplayManager.h"
#include "ResourceManager.h"

#include "LevelIntro.h"
#include "Saucer.h"
#include "GameOver.h"

LevelIntro::LevelIntro() : Level("level_intro") {

}

void LevelIntro::initialize() {
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

	Platform *platform = new Platform(df::Vector(7, 65));
	new Platform(df::Vector(70, 55));
	new Platform(df::Vector(133, 60));
	new Platform(df::Vector(196, 40));
	new Platform(df::Vector(250, 40));
	new Platform(df::Vector(304, 40));
	new Platform(df::Vector(304, 60));
	new Platform(df::Vector(360, 50));
	new Platform(df::Vector(414, 50));


	df::Object * text_1 = new df::Object();
	text_1->setSprite(RM.getSprite("intro_1"), true);
	text_1->setSolidness(df::SPECTRAL);
	text_1->setPosition(df::Vector(90, 20));
	text_1->setType("Instruction");

	df::Object * text_2 = new df::Object();
	text_2->setSprite(RM.getSprite("intro_2"), true);
	text_2->setSolidness(df::HARD);
	text_2->setPosition(df::Vector(270, 20));
	text_2->setType("Instruction");

	shooting_instruction = new df::Object();
	shooting_instruction->setSprite(RM.getSprite("intro_3"), true);
	shooting_instruction->setSolidness(df::SPECTRAL);
	shooting_instruction->setPosition(df::Vector(500, 15));
	shooting_instruction->setType("Instruction");

}

void LevelIntro::levelLogic() {
	static int count_down = 150;
	float x = hero->getPosition().getX();
	switch (progress) {
	case 0:
	{
		if (x >= 360) {
			progress = 1;
			Saucer* saucer = new Saucer(10, 0, 0, 0, 0);
			saucer->setPosition(df::Vector(550, 35));
		}
		break;
	}
	case 1:
	{
		df::ObjectList saucers = WM.objectsOfType("Saucer");
		if (saucers.isEmpty()) {
			progress = 2;
			shooting_instruction->setSprite(RM.getSprite("intro_4"), true);
			//Saucer* saucer = new Saucer(10, 0, 0, 1, 0);
			//saucer->setPosition(df::Vector(550, 35));
		}
		break;
	}
	case 2:
	{
		count_down--;
		if (count_down <= 0) {
			progress = 3;
			shooting_instruction->setSprite(RM.getSprite("intro_5"), true);
			Saucer* saucer = new Saucer(10, 0, 0, 0, 0);
			saucer->setPosition(df::Vector(530, 35));
			Saucer* saucer1 = new Saucer(10, 0, 0, 0, 0);
			saucer1->setPosition(df::Vector(550, 35));
			Saucer* saucer2 = new Saucer(10, 0, 0, 0, 0);
			saucer2->setPosition(df::Vector(570, 35));
		}
		break;
	}
	case 3:
	{
		df::ObjectList saucers = WM.objectsOfType("Saucer");
		if (saucers.isEmpty()) {
			progress = 4;
			shooting_instruction->setSprite(RM.getSprite("intro_6"), true);
			Saucer* saucer = new Saucer(10, 0, 0, 0, 0);
			saucer->setPosition(df::Vector(530, 35));
			saucer->setSprite(RM.getSprite("intro_small"), true);
			saucer->setSpriteSlowdown(7);
			Saucer* saucer1 = new Saucer(10, 0, 0, 0, 0);
			saucer1->setPosition(df::Vector(560, 35));
			saucer1->setSprite(RM.getSprite("intro_small"), true);
			saucer1->setSpriteSlowdown(7);
		}
		break;
	}
	case 4:
	{
		df::ObjectList saucers = WM.objectsOfType("Saucer");
		if (saucers.isEmpty()) {
			progress = 5;
			shooting_instruction->setSprite(RM.getSprite("intro_7"), true);
			Saucer* saucer = new Saucer(10, 0, 0, 0, 0);
			saucer->setPosition(df::Vector(530, 35));
			Saucer* saucer1 = new Saucer(10, 0, 0, 0, 0);
			saucer1->setPosition(df::Vector(525, 35));
			Saucer* saucer2 = new Saucer(10, 0, 0, 0, 0);
			saucer2->setPosition(df::Vector(530, 40));
			Saucer* saucer3 = new Saucer(10, 0, 0, 0, 0);
			saucer3->setPosition(df::Vector(530, 35));
			Saucer* saucer4 = new Saucer(10, 0, 0, 0, 0);
			saucer4->setPosition(df::Vector(540, 35));
		}
		break;
	}
	case 5:
	{
		df::ObjectList saucers = WM.objectsOfType("Saucer");
		if (saucers.isEmpty()) {
			progress = 6;
			levelComplete();
		}
	}
	}
}

int LevelIntro::eventHandler(const df::Event *p_e) {
	Level::eventHandler(p_e);
	if (started) {

	}

	return 0;
}
