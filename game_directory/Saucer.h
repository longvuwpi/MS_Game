//
// Saucer.h
//

#include "Object.h"
#include "EventCollision.h"
 
class Saucer : public df::Object {
 private:
  void moveToStart();
  void out();
  void die();
  void hit(const df::EventCollision *p_collision_event);
  int health;

 public:
  Saucer(int health);
  ~Saucer();
  void draw();
  int eventHandler(const df::Event *p_e);
  int getHealth();
  void takeDamage(df::Vector at, int damage);
};
