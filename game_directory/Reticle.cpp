//
// Reticle.cpp
//
#include <iostream>
#include <stdio.h>
#include <cmath>

// Engine includes.
#include "EventMouse.h"
#include "DisplayManager.h"
#include "WorldManager.h"
#include "EventStep.h"
#include "ObjectList.h"
#include "ObjectListIterator.h"
#include "utility.h"

// Game includes.
#include "Reticle.h"
#include "Weapon.h"

Reticle::Reticle(Hero* owner) {
	expand_size = 0;
	hero = owner;
	setType("Reticle");
	setSolidness(df::SPECTRAL);
	setAltitude(df::MAX_ALTITUDE); // Make Reticle in foreground.

	// Reticle moves with mouse, so register.
	registerInterest(df::MSE_EVENT);
	registerInterest(df::STEP_EVENT);

	// Start reticle in center of window.
	df::Vector p(WM.getBoundary().getHorizontal() / 2,
		WM.getBoundary().getVertical() / 2);
	setPosition(p);
}

// Handle event.
// Return 0 if ignored, else 1.
int Reticle::eventHandler(const df::Event *p_e) {

	if (p_e->getType() == df::MSE_EVENT) {
		const df::EventMouse *p_mouse_event = dynamic_cast <const df::EventMouse *> (p_e);
		if (p_mouse_event->getMouseAction() == df::MOVED) {
			// Change location to new mouse position.
			setPosition((*(new df::Vector(WM.getView().getCorner().getX(), 0))) + p_mouse_event->getMousePosition());
			return 1;
		}
	}

	if (p_e->getType() == df::STEP_EVENT) {
		if (expand_size > 0) {
			expand_size -= 0.1f;
		}
		else {
			expand_size = 0;
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
		DM.drawCh(getPosition() + df::Vector(0, -0.7) + df::Vector(0, -expand_size), '|', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(0, 0.9) + df::Vector(0, expand_size), '|', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(-1.4f, -0.3f) + df::Vector(-expand_size, 0), '_', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(1.4f, -0.3f) + df::Vector(expand_size, 0), '_', df::GREEN);
		break;
	case WeaponType::LAUNCHER:
		DM.drawCh(getPosition() + df::Vector(-2 - expand_size, -1.5 - expand_size), '\\', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(-2 - expand_size, 1.5 + expand_size), '/', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(2 + expand_size, 1.5 + expand_size), '\\', df::GREEN);
		DM.drawCh(getPosition() + df::Vector(2 + expand_size, -1.5 - expand_size), '/', df::GREEN);
		break;
	case WeaponType::SNIPER:
		if ((hero->getCurrentWeapon()->isScoping()) && (hero->getCurrentWeapon()->canShoot())) {

			df::Vector pixel_pos = df::spacesToPixels(df::worldToView(getPosition()));
			int pixel_scope_radius = 300;
			float scope_radius = ((float)pixel_scope_radius / (float)DM.getHorizontalPixels()) * DM.getHorizontal();
			std::cout << "Scope radius is " << scope_radius;
			//set invisible all objects in the world that are out of scope
			df::ObjectList list_in_world = WM.getAllObjects();
			df::ObjectListIterator liw(&list_in_world);
			liw.first();
			while (!liw.isDone()) {
				df::Vector view_pos = df::worldToView(liw.currentObject()->getPosition());
				int horizontal = liw.currentObject()->getBox().getHorizontal();
				int vertical = liw.currentObject()->getBox().getVertical();
				df::Vector pixel_object_corner1 = df::spacesToPixels(view_pos + df::Vector(-horizontal / 2, -vertical / 2));
				df::Vector pixel_object_corner2 = df::spacesToPixels(view_pos + df::Vector(-horizontal / 2, vertical / 2));
				df::Vector pixel_object_corner3 = df::spacesToPixels(view_pos + df::Vector(horizontal / 2, vertical / 2));
				df::Vector pixel_object_corner4 = df::spacesToPixels(view_pos + df::Vector(horizontal / 2, -vertical / 2));

				if (((pixel_object_corner1 - pixel_pos).getMagnitude() > (pixel_scope_radius * 2)) &&
					((pixel_object_corner2 - pixel_pos).getMagnitude() > (pixel_scope_radius * 2)) &&
					((pixel_object_corner3 - pixel_pos).getMagnitude() > (pixel_scope_radius * 2)) &&
					((pixel_object_corner4 - pixel_pos).getMagnitude() > (pixel_scope_radius * 2)) &&
					(liw.currentObject()->getType() != "DamageIndicator") && 
					(liw.currentObject()->getType() != "Fraps") && 
					(liw.currentObject()->getType() != "Points") && 
					(liw.currentObject()->getType() != "ViewObject")) {
					liw.currentObject()->setVisible(false);
				}
				else {
					liw.currentObject()->setVisible(true);
				}
				liw.next();
			}

			for (int i = 0; i < WM.getView().getHorizontal(); i++) {
				for (int j = 0; j < WM.getView().getVertical(); j++) {
					df::Vector space_current(i, j);
					df::Vector pixel_current = df::spacesToPixels(space_current);
					df::Vector current = pixel_current - pixel_pos;
					df::Vector corner = WM.getView().getCorner();

					if (floor(current.getMagnitude()) > pixel_scope_radius) {
						DM.drawCh(space_current + corner, ' ', df::GREEN);
					}
					else if ((floor(current.getMagnitude()) == pixel_scope_radius) || 
						(ceil(current.getMagnitude()) == pixel_scope_radius) || 
						(floor((space_current + corner - getPosition()).getMagnitude()) == (int)scope_radius) || 
						(ceil((space_current + corner - getPosition()).getMagnitude()) == (int)scope_radius)) {
						DM.drawCh(space_current + corner, '*', df::GREEN);
					}
					else {
						if (((int)(i + corner.getX()) == (int)getPosition().getX()) && (j == (int)getPosition().getY())) {
							DM.drawCh(space_current + corner, '+', df::GREEN);
						}
						else if (i + (int)corner.getX() == (int)getPosition().getX()) {
							DM.drawCh(space_current + corner, '|', df::GREEN);
						}
						else if (j == (int)getPosition().getY()) {
							DM.drawCh(space_current + corner + df::Vector(0, -0.3f), '_', df::GREEN);
						}
					}
				}
			}
		}
		else {
			df::ObjectList list_in_window = WM.getAllObjects();
			df::ObjectListIterator li(&list_in_window);
			li.first();
			while (!li.isDone()) {
				if (li.currentObject()->getType() != "Weapon") {
					li.currentObject()->setVisible(true);
				}
				li.next();
			}
			hero->getCurrentWeapon()->setVisible(true);
			DM.drawCh(getPosition() + df::Vector(0, -4.7) + df::Vector(0, -expand_size), '|', df::GREEN);
			DM.drawCh(getPosition() + df::Vector(0, 4.9) + df::Vector(0, expand_size), '|', df::GREEN);
			DM.drawCh(getPosition() + df::Vector(-5.4f, -0.3f) + df::Vector(-expand_size, 0), '_', df::GREEN);
			DM.drawCh(getPosition() + df::Vector(5.4f, -0.3f) + df::Vector(expand_size, 0), '_', df::GREEN);
		}
		break;
	default:
		break;
	}


}

void Reticle::expand() {
	expand_size = 1.1f;
}


