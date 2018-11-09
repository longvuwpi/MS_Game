#include <stdlib.h>		// for rand()
#include <iostream>
#include <stdio.h>

#include "EnemyMovement.h"

#include "LogManager.h"
#include "WorldManager.h"
#include "GameManager.h"


EnemyMovement::EnemyMovement(Saucer *saucer) {
	owner = saucer;
	angle = 0;
	y_velocity = 0.3f;
}

void EnemyMovement::move() {
	switch (owner->getMovementType()) {
	case 0:
	{
		float vert = WM.getView().getVertical();
		float pos_y = owner->getPosition().getY();
		std::cout << "Vert bound is " << vert << ", pos y is " << pos_y;
		if ((pos_y < (vert / 3)) || (pos_y > (vert * 2 / 3))) {
			y_velocity = -y_velocity;
		}
		owner->setVelocity(df::Vector(0, y_velocity));
		break;
	}
	case 1:
	{
		angle += 5;
		if (angle >= 360) angle = 0;

		//LM.writeLog("Saucer moving");
		float delta_x = sin(angle * 3.14159265 / 180) * 15 - (GM.getStepCount() - move_to_start_frame)*(0.2);
		float delta_y = sin((90 - angle)*3.14159265 / 180) * 6;
		owner->setPosition(move_to_start_pos + df::Vector(delta_x, delta_y));
		//std::cout << "Saucer Pos: " << delta_x << "," << delta_y << "\n";
		break;
	}
	case 2:
	{
		df::Vector saucerPos = owner->getPosition();

		df::ObjectList object_list = WM.objectsOfType("Hero");
		if (object_list.getCount() > 0) {
			df::ObjectListIterator li(&object_list);
			li.first();
			df::Vector hero_pos = li.currentObject()->getPosition();
			df::Vector direction = hero_pos - saucerPos;

			if (direction.getMagnitude() <= 100) {
				direction.normalize();
				owner->setVelocity(direction);
			}
			else {
				owner->setVelocity(df::Vector(-1, 0));
			}
		}
		break;
	}
	case 3:
	{
		angle += 9;
		if (angle >= 360) angle = 0;

		float delta_x = -(GM.getStepCount() - move_to_start_frame);
		float delta_y = sin(angle*3.14159265 / 180) * 6;
		owner->setPosition(move_to_start_pos + df::Vector(delta_x, delta_y));

		break;
	}
	default:
		break;
	}

	return;
}

void EnemyMovement::moveToStart() {
	df::Vector temp_pos;
	df::Vector view_corner = WM.getView().getCorner();

	// Get world boundaries.
	int world_horiz = (int)WM.getView().getHorizontal();
	int world_vert = (int)WM.getView().getVertical();

	// x is off right side of window.
	//temp_pos.setX(world_horiz + rand() % (int)world_horiz);
	temp_pos.setX(view_corner.getX() + world_horiz - 10);
	// y is in vertical range.
	temp_pos.setY(rand() % (int)(world_vert - 4) + 4.0f);

	// If collision, move down slightly until empty space.
	df::ObjectList collision_list = WM.isCollision(owner, temp_pos);
	while (!collision_list.isEmpty()) {
		temp_pos.setY(temp_pos.getY() + 1);
		collision_list = WM.isCollision(owner, temp_pos);
	}

	WM.moveObject(owner, temp_pos + (*(new df::Vector(WM.getView().getCorner().getX(), 0))));
	markStart();
}

void EnemyMovement::markStart() {
	move_to_start_frame = GM.getStepCount();
	move_to_start_pos = owner->getPosition();
}