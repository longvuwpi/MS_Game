#pragma once

#include "Level.h"

class LevelIntro : public Level {
private:
	df::Object *shooting_instruction;
	void flashInstruction();
public:
	LevelIntro();
	void levelLogic() override;
	void initialize() override;	
	int eventHandler(const df::Event *p_e);
};