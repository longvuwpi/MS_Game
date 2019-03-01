#pragma once

#include "Vector.h"

#include "Saucer.h"

class Saucer;

class EnemyMovement {
private:
	Saucer *owner;
	int move_to_start_frame;
	df::Vector move_to_start_pos;

	//for type 0
	float y_velocity;

	//for type 1
	float angle;
public:
	EnemyMovement(Saucer *saucer);
	void move();
	void moveToStart();
	void markStart();
};