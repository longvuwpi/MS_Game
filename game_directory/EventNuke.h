//
// EventNuke.h
//

#include "Event.h"
#include "Vector.h"

const std::string NUKE_EVENT =  "nuke";

class EventNuke : public df::Event {
private:
	df::Vector origin;
	float radius;
	int damage;
 public:
  EventNuke(df::Vector origin, float radius, int damage);
  df::Vector getOrigin() const;
  float getRadius() const;
  int getDamage() const;
};
