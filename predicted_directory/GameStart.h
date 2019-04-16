//
// GameStart.h
//
#pragma once

#include "Music.h"
#include "ViewObject.h"

class GameStart : public df::ViewObject {

private:
	df::Music *p_music;

public:
	GameStart();
	int eventHandler(const df::Event *p_e);
	void draw();
	void playMusic();
	void start();

};
