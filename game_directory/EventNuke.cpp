//
// EventNuke.cpp
//

#include "EventNuke.h"
#include "Vector.h"

EventNuke::EventNuke(df::Vector nukeOrigin, float nukeRadius) {
  setType(NUKE_EVENT);
  origin = nukeOrigin;
  radius = nukeRadius;
};

df::Vector EventNuke::getOrigin() const {
	return origin;
}

float EventNuke::getRadius() const {
	return radius;
}
