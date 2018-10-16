#pragma once

#include "Object.h"

class AmmoRefill : public df::Object {
public:
	AmmoRefill();
	int eventHandler(const df::Event *p_e);
	void draw();
};