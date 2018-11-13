#pragma once

#include "Level.h"

class Level1 : public Level {
public:
	Level1();
	void levelLogic() override;
	void initialize() override;
};