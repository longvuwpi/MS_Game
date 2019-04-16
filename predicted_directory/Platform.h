//
//
//
#pragma once
#include "Object.h"
#include "Vector.h"

class Platform : public df::Object {

 public:
	 Platform(df::Vector pos);
	 ~Platform();
	 int eventHandler(const df::Event *p_e);
};
