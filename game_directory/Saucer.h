//
// Saucer.h
//

#include "Object.h"
#include "EventCollision.h"

enum EnemyType {MINION, BOSS, WEAKPOINT};

class Saucer : public df::Object {
private:
	EnemyType enemy_type;
	int max_health;
	int health;
	int damage;
	int fire_count_down;
	float bullet_radius;
	void moveToStart();
	void out();
	void die();
	void hit(const df::EventCollision *p_collision_event);
	df::Sprite *bullet_sprite;
	void fire();
	void step();

public:
	Saucer(int health, int dmg, float radius);
	~Saucer();
	//void draw();
	int eventHandler(const df::Event *p_e);
	int getHealth();
	void takeDamage(df::Vector at, int damage);
	EnemyType getEnemyType();
	void setEnemyType(EnemyType enemyType);
};
