#pragma once

#include "Object.h"

class ItemPick : public df::Object {
private:
    int regeneration;

public:
	HealthPick();
    AmmoRefill();
	int eventHandler(const df::Event *p_e);
	void draw();
};