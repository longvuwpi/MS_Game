#pragma once

#include "Object.h"
#include "ViewObject.h"
#include "Hero.h"

class AmmoDisplay : public df::ViewObject {

private:
	void step();
	Hero *owner;
	//Loaded ammo digits
	Object *loaded_1;
	Object *loaded_2;
	//Slash character
	Object *slash;
	//Backup ammo digits
	Object *backup_1;
	Object *backup_2;
public:
	AmmoDisplay(Hero *hero);
	int eventHandler(const df::Event *p_e);
	void positionSprites();
	void draw();
};
