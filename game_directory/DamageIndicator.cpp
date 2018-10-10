#include "DamageIndicator.h"

#include "EventStep.h"
#include "WorldManager.h"
#include "DisplayManager.h"

DamageIndicator::DamageIndicator(df::Vector at, int dmg) {
	setType("DamageIndicator");
	setPosition(at);
	damage = dmg;
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