
#include "Event.h"
#include "EventPlayerJumping.h"
#include "Vector.h"

EventPlayerJumping::EventPlayerJumping(df::Vector new_player_pos) {
	playerPos = new_player_pos;
	setType(PLAYER_JUMPING_EVENT);
};

df::Vector EventPlayerJumping::getPlayerPos() {
	return playerPos;
}
