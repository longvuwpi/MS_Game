//
// Saucer.h
//
#pragma once
#include "Object.h"
#include "EventCollision.h"

enum EnemyType {MINION, BOSS, WEAKPOINT};

class Saucer : public df::Object {
private:
    //Hero * hero;
    //Saucer * saucer;
	EnemyType enemy_type;
	int max_health;
	int damage;
	int fire_count_down;
	int movement_type;
	int move_to_start_frame;
	df::Vector move_to_start_pos;
	float bullet_radius;
	void moveToStart();
	void out();
	void hit(const df::EventCollision *p_collision_event);
	df::Sprite *bullet_sprite;
	void fire();
    //void move();

protected:
	int health;	
	virtual void step();
	virtual void die();
	virtual void move();
public:
	Saucer(int health, int dmg, float radius, int movementType = 0);
	~Saucer();
	//void draw();
	int eventHandler(const df::Event *p_e);
	int getHealth();
	virtual void takeDamage(df::Vector at, int damage);
	EnemyType getEnemyType();
	void setEnemyType(EnemyType enemyType);
   
};
