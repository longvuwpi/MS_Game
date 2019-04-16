//
// EventNuke.cpp
//

#include "EventNuke.h"
#include "Vector.h"

EventNuke::EventNuke(df::Vector nukeOrigin, float nukeRadius, int dmg) {
  setType(NUKE_EVENT);
  origin = nukeOrigin;
  radius = nukeRadius;
  damage = dmg;
};

df::Vector EventNuke::getOrigin() const {
	return origin;
}

float EventNuke::getRadius() const {
	return radius;
}

int EventNuke::getDamage() const {
	return damage;
}
