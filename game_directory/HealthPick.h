#pragma once

#include "Object.h"

class HealthPick : public df::Object {
private:
    int regeneration;

public:
	HealthPick();
	int eventHandler(const df::Event *p_e);
	void draw();
};