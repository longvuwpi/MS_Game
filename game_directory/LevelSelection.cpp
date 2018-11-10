#include "ResourceManager.h"
#include "LogManager.h"
#include "DisplayManager.h"
#include "EventMouse.h"
#include "EventStep.h"

#include "LevelSelection.h"
#include "Level.h"

LevelSelection::LevelSelection() {
	setType("LevelSelection");

	df::Sprite *p_temp_sprite = RM.getSprite("level_selection");
	if (!p_temp_sprite)
		LM.writeLog("GameStart::GameStart(): Warning! Sprite 'level selection' not found");
	else {
		setSprite(p_temp_sprite);
		//setSpriteSlowdown(15);
	}

	registerInterest(df::MSE_EVENT);
	registerInterest(df::STEP_EVENT);

	setLocation(df::TOP_CENTER);
	mouse_pos = df::Vector(0, 0);

	int display_horiz, display_vert;
	display_horiz = DM.getHorizontal();
	display_vert = DM.getVertical();

	Level *level_intro = new Level();
	level_intro->setPosition(df::Vector(display_horiz * 2 / 5, display_vert / 3));
	//Level *level_1 = new Level();
	//level_1->setPosition(df::Vector(display_horiz * 2 / 5, display_vert * 2 / 3));
}

void LevelSelection::draw() {
	df::Object::draw();
}

int LevelSelection::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::MSE_EVENT) {
		const df::EventMouse *p_mouse_event = dynamic_cast <const df::EventMouse *> (p_e);
		if (p_mouse_event->getMouseAction() == df::MOVED) {
			mouse_pos = p_mouse_event->getMousePosition();
			return 1;
		}
	}

	if (p_e->getType() == df::STEP_EVENT) {
		DM.drawCh(mouse_pos + df::Vector(0, -0.2f), RETICLE_CHAR, df::GREEN);
		DM.drawCh(mouse_pos + df::Vector(0, -0.7), '|', df::GREEN);
		DM.drawCh(mouse_pos + df::Vector(0, 0.9), '|', df::GREEN);
		DM.drawCh(mouse_pos + df::Vector(-1.4f, -0.3f), '_', df::GREEN);
		DM.drawCh(mouse_pos + df::Vector(1.4f, -0.3f), '_', df::GREEN);

		return 1;
	}
	return 0;

}