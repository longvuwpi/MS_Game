//
// Reticle.cpp
//

// Engine includes.
#include "EventMouse.h"
#include "DisplayManager.h"
#include "WorldManager.h"
#include "EventStep.h"

// Game includes.
#include "Reticle.h"
#include "Weapon.h"

Reticle::Reticle(Hero* owner) {
	expandSize = 0;
	hero = owner;
	setType("Reticle");
	setSolidness(df::SPECTRAL);
	setAltitude(df::MAX_ALTITUDE); // Make Reticle in foreground.

	// Reticle moves with mouse, so register.
	registerInterest(df::MOUSE_EVENT);
	registerInterest(df::STEP_EVENT);
	// Start reticle in center of window.
	df::Vector p(WM.getBoundary().getHorizontal() / 2,
		WM.getBoundary().getVertical() / 2);
	setPosition(p);
}

// Handle event.
// Return 0 if ignored, else 1.
int Reticle::eventHandler(const df::Event *p_e) {

	if (p_e->getType() == df::MOUSE_EVENT) {
		const df::EventMouse *p_mouse_event = dynamic_cast <const df::EventMouse *> (p_e);
		if (p_mouse_event->getMouseAction() == df::MOVED) {
			// Change location to new mouse position.
			setPosition((*(new df::Vector(WM.getView().getCorner().getX(), 0))) + p_mouse_event->getMousePosition());
			return 1;
		}
	}

	if (p_e->getType() == df::STEP_EVENT) {
		if (expandSize > 0) {
			expandSize -= 0.1f;
		}
		else {
			expandSize = 0;
		}
	}

	// If get here, have ignored this event.
	return 0;
}

// Draw reticle on window.
void Reticle::draw() {
	switch (hero->getCurrentWeapon()->getWeaponType()) {
	case WeaponType::RIFLE: 
		DM.drawCh(getPosition() + df::Vector(0, -0.2f), RETICLE_CHAR, df::GREEN);
		DM.drawCh(getPosition() + df::Vector(0, -0.7) + df::Vector(0, -expandSize), '|', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(0, 0.9) + df::Vector(0, expandSize), '|', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(-1.4f, -0.3f) + df::Vector(-expandSize, 0), '_', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(1.4f, -0.3f) + df::Vector(expandSize, 0), '_', df::GREEN);
		break;
	case WeaponType::LAUNCHER:
		DM.drawCh(getPosition() + df::Vector(-2 - expandSize, -1.5 - expandSize), '\\', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(-2 - expandSize,  1.5 + expandSize), '/', df::GREEN);
		DM.drawCh(getPosition() + df::Vector( 2 + expandSize,  1.5 + expandSize), '\\', df::GREEN);
		DM.drawCh(getPosition() + df::Vector( 2 + expandSize, -1.5 - expandSize), '/', df::GREEN);
		break;
	case WeaponType::SNIPER:
		DM.drawCh(getPosition() + df::Vector(0, -2.7) + df::Vector(0, -expandSize), '|', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(0, 2.9) + df::Vector(0, expandSize), '|', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(-3.4f, -0.3f) + df::Vector(-expandSize, 0), '_', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(3.4f, -0.3f) + df::Vector(expandSize, 0), '_', df::GREEN);
		break;
	default:
		break;
	}


}

void Reticle::expand() {
	expandSize = 1.1f;
}
