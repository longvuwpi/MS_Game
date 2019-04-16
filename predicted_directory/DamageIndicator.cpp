#include "DamageIndicator.h"

#include "EventStep.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "util.h"
#include "LogManager.h"

DamageIndicator::DamageIndicator(df::Vector at, int dmg) {
	setType("DamageIndicator");
	setPosition(at);
	damage = dmg;
	di_modified = true;
	life_time = 15;
	setSolidness(df::SOFT);
	setAltitude(df::MAX_ALTITUDE);
	registerInterest(df::STEP_EVENT);
}

DamageIndicator::DamageIndicator() {
	setType("DamageIndicator");
	life_time = 15;
	setSolidness(df::SOFT);
	setAltitude(df::MAX_ALTITUDE);
	registerInterest(df::STEP_EVENT);
}

int DamageIndicator::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::STEP_EVENT) {
		if (life_time <= 0) {
			WM.markForDelete(this);
		}
		life_time--;
		return 1;
	}
}

void DamageIndicator::draw() {
	DM.drawString(getPosition(), std::to_string(-damage), df::CENTER_JUSTIFIED, df::WHITE);
}

std::string DamageIndicator::serialize(std::string all) {
	//LM.writeLog(1, "DamageIndicator::serialize() %s", all);

	// Do main serialize from parent.
	std::string s = Object::serialize(all);

	// Add Weapon-specific attributes.
	if ((all == "ALL") || di_modified) {
		LM.writeLog("DamageIndicator::serialize(): serializing dmg %d", damage);
		s += "damage:" + df::toString(damage) + ",";
		di_modified = false;
	}

	return s;
}

int DamageIndicator::deserialize(std::string str) {
	//LM.writeLog("DamageIndicator::deserialize()");

	// Do main deserialize from parent.
	Object::deserialize(str);

	// Get ready for parsing.
	std::string val;
	df::match(str, "");

	val = df::match("", "damage");
	if (!val.empty()) {
		int i = atoi(val.c_str());
		LM.writeLog("DamageIndicator::deserialize(): damage is %d", i);
		damage = i;
	}

	return 0;
}

bool DamageIndicator::isModified() const {
	if (di_modified) return true;
	return Object::isModified();
}