//
// Explosion.h
//

#include "Object.h"

class Explosion : public df::Object {

 private:
  int time_to_live;
  void step();
  float radius;

 public:
  Explosion(std::string sprite, float radius);
  int eventHandler(const df::Event *p_e);
  void draw();
};
