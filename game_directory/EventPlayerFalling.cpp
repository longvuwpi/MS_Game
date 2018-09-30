//
// EventNuke.cpp
//

#include "Event.h"
#include "EventPlayerFalling.h"
#include "Vector.h"

EventPlayerFalling::EventPlayerFalling(df::Vector new_player_pos) {
	playerPos = new_player_pos;
	setType(PLAYER_FALLING_EVENT);
};

df::Vector EventPlayerFalling::getPlayerPos() const {
	return playerPos;
}
