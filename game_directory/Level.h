#pragma once

//Engine includes
#include "Object.h"
#include "ObjectList.h"

//Game includes
#include "Hero.h"

class Level : public df::Object {
protected:
	Hero * hero;
	df::ObjectList landscape;
	int progress;
	int start_frame;
	bool started;
	bool bossSpawned;
	void levelComplete();
public:
	Level(std::string level_name);
	void start();
	void draw();
	int eventHandler(const df::Event *p_e);
	void step();
	virtual void levelLogic();
	virtual void initialize();
};