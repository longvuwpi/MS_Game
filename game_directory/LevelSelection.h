#pragma once
#include "ViewObject.h"

class LevelSelection : public df::ViewObject {

private:
	//void start();
	df::Vector mouse_pos;
public:
	LevelSelection();
	int eventHandler(const df::Event *p_e);
	void draw();
	//void playMusic();
};
