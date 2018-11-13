//
// Saucer.h
//
#pragma once
#include "Object.h"
#include "EventCollision.h"
#include "EnemyMovement.h"
#include "EnemyAttack.h"

class EnemyMovement;
class EnemyAttack;

enum EnemyType {MINION, BOSS, WEAKPOINT};

class Saucer : public df::Object {
private:
	EnemyType enemy_type;
	EnemyMovement *enemy_movement;
	EnemyAttack *enemy_attack;
	int max_health;
	int damage;
	int fire_count_down;
	int movement_type;
	int attack_type;
	float bullet_radius;
	void out();
	void hit(const df::EventCollision *p_collision_event);
	df::Sprite *bullet_sprite;
protected:
	int health;	
	virtual void step();
	virtual void die();
public:
	Saucer(int health, int dmg, float radius, int movementType = 0, int attackType = 0);
	virtual void takeDamage(df::Vector at, int damage);
	~Saucer();
	//void draw();
	int eventHandler(const df::Event *p_e);
	int getHealth();
	int getMovementType();
	int getAttackType();
	EnemyType getEnemyType();
	void setEnemyType(EnemyType enemyType);
	int getDamage();
	float getBulletRadius();
	df::Sprite * getBulletSprite();
	void markStart();
};
