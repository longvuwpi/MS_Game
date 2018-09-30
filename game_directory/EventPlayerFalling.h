//
// EventNuke.h
//

#include "Event.h"
#include "Vector.h"

const std::string PLAYER_FALLING_EVENT =  "playerfalling";

class EventPlayerFalling : public df::Event {
 private:
	 df::Vector playerPos;
 public:
  EventPlayerFalling(df::Vector new_player_pos);
  df::Vector getPlayerPos() const;
};
