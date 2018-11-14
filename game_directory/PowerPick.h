#pragma once

#include "Object.h"

class PowerPick : public df::Object {
public:
	PowerPick();
	int eventHandler(const df::Event *p_e);
	void draw();
};