//
// Explosion.h
//

#include "Object.h"

class Explosion : public df::Object {

 private:
  int time_to_live;
  void step();

 public:
  Explosion(std::string sprite);
  int eventHandler(const df::Event *p_e);
};
