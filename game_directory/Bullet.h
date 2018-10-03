//
// Bullet.h
//

#include "EventCollision.h"
#include "Object.h"
#include "Sprite.h"

class Bullet : public df::Object {

 private:
  void out();
  void hit(const df::EventCollision *p_collision_event);
  float weight;
  float radius_of_effect;
  bool affected_by_gravity;
  void step();

 public:
	 Bullet(df::Vector hero_pos, df::Sprite *sprite, bool affectedByGravity, float bulletWeight, float radius_of_effect);
  int eventHandler(const df::Event *p_e);
};
