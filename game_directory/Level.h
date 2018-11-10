#pragma once

//Engine includes
#include "Object.h"
#include "ObjectList.h"

//Game includes
#include "Hero.h"

class Level : public df::Object {
private:
	Hero * hero;
	df::ObjectList landscape;
	int progress;
	bool started;
	bool bossSpawned;
public:
	Level();
	void start();
	void draw();
	int eventHandler(const df::Event *p_e);
};