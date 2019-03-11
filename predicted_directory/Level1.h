#pragma once

#include "Level.h"
#include "Boss.h"

class Level1 : public Level {
private:
	Boss * boss;
public:
	Level1();
	void levelLogic() override;
	void initialize() override;
};