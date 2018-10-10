#pragma once

//Engine includes
#include "Object.h"

//Game includes
#include "Hero.h"

class Level : public df::Object {
private:
	Hero * hero;
public:
	Level();
	void start();
	void draw();
	int eventHandler(const df::Event *p_e);
};