//
// EventNuke.h
//

#include "Event.h"
#include "Vector.h"

const std::string PLAYER_JUMPING_EVENT = "playerjumping";

class EventPlayerJumping : public df::Event {
private:
	df::Vector playerPos;
public:
	EventPlayerJumping(df::Vector new_player_pos);
	df::Vector getPlayerPos();
};
