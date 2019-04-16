#pragma once
#include "Object.h"
#include "Event.h"
#include "Vector.h"

class DamageIndicator : public df::Object {
private:
	int life_time;
	int damage;
	bool di_modified;
public:
	DamageIndicator(df::Vector at, int damage);
	DamageIndicator();
	int eventHandler(const df::Event *p_e);
	void draw();
	bool isModified() const override;
	std::string serialize(std::string all = "");
	int deserialize(std::string str) override;
};