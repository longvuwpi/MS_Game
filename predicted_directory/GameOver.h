//
// GameOver.h
//

#include "ViewObject.h"

class GameOver : public df::Object {

 private:
  int time_to_live;
  void step();

 public:
  GameOver(df::Vector position, bool win);
  GameOver();
  ~GameOver();
  int eventHandler(const df::Event *p_e);
  void draw();
  int deserialize(std::string str) override;

};
