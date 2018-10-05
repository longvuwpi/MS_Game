//
// Saucer.h
//

#include "Object.h"
#include "EventCollision.h"
 
class Saucer : public df::Object {
 private:
  int health;
  void moveToStart();
  void out();
  void hit(const df::EventCollision *p_collision_event);
  //void detectPlayer(const df::EventPath *p_path_event);
 
 public:
  Saucer();
  ~Saucer();
  int eventHandler(const df::Event *p_e);
  int getHealth();
};
