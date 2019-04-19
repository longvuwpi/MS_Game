#include <iostream>
#include <stdio.h>

#include "ResourceManager.h"
#include "SceneGraph.h"
#include "EventStep.h"
#include "DisplayManager.h"
#include "ObjectList.h"
#include "WorldManager.h"
#include "LogManager.h"
#include "NetworkManager.h"

#include "Hero.h"
#include "AmmoDisplay.h"
#include "Weapon.h"

AmmoDisplay::AmmoDisplay(Hero *hero) {
	owner = hero;
	setSprite(RM.getSprite("ammo_display"));

	//initialize the digits objects
	slash = new df::Object();
	loaded_1 = new df::Object();
	loaded_2 = new df::Object();
	backup_1 = new df::Object();
	backup_2 = new df::Object();

	//set initial sprites
	slash->setSprite(RM.getSprite("slash"));
	loaded_1->setSprite(RM.getSprite("green_0"));
	loaded_2->setSprite(RM.getSprite("green_0"));
	backup_1->setSprite(RM.getSprite("green_0"));
	backup_2->setSprite(RM.getSprite("green_0"));

	positionSprites();

	//set to max altitude so that the digits are drawn on top
	slash->setAltitude(df::MAX_ALTITUDE);
	loaded_1->setAltitude(df::MAX_ALTITUDE);
	loaded_2->setAltitude(df::MAX_ALTITUDE);
	backup_1->setAltitude(df::MAX_ALTITUDE);
	backup_2->setAltitude(df::MAX_ALTITUDE);

	loaded_1->setVisible(false);
	loaded_2->setVisible(false);
	backup_1->setVisible(false);
	backup_2->setVisible(false);

	setType("AmmoDisplay");
	loaded_1->setType("AmmoDisplay");
	loaded_2->setType("AmmoDisplay");
	backup_1->setType("AmmoDisplay");
	backup_2->setType("AmmoDisplay");
	slash->setType("AmmoDisplay");

	registerInterest(df::STEP_EVENT);
}

int AmmoDisplay::eventHandler(const df::Event *p_e) {
	if (p_e->getType() == df::STEP_EVENT) {
		if (!NM.isServer()) {
			step();
			return 1;
		}
	}

	// If get here, have ignored this event.
	return 0;
}

void AmmoDisplay::step() {
	df::ObjectList hero_list = WM.objectsOfType("Hero");
	if ((!hero_list.isEmpty()) && (owner->getCurrentWeapon() != NULL)) {
		int loaded_ammo = owner->getCurrentWeapon()->getAmmoLoaded();
		int backup_ammo = owner->getCurrentWeapon()->getAmmoBackup();
		int loaded_ammo_max = owner->getCurrentWeapon()->getAmmoLoadedMax();
		int backup_ammo_max = owner->getCurrentWeapon()->getAmmoBackupMax();

		std::string loadedColor;
		std::string backupColor;

		float normalizedloaded = (float)loaded_ammo / (float)loaded_ammo_max;
		if (normalizedloaded > 0.7f)
		{
			loadedColor = "green_";
		}
		else if (normalizedloaded > 0.3f)
		{
			loadedColor = "yellow_";
		}
		else
		{
			loadedColor = "red_";
		}

		float normalizedbackup = (float)backup_ammo / (float)backup_ammo_max;
		if (normalizedbackup > 0.7f)
		{
			backupColor = "green_";
		}
		else if (normalizedbackup > 0.3f)
		{
			backupColor = "yellow_";
		}
		else
		{
			backupColor = "red_";
		}

		std::string loaded1Name, loaded2Name, backup1Name, backup2Name;
		df::Sprite *loaded1Sprite, *loaded2Sprite, *backup1Sprite, *backup2Sprite;

		int loaded1 = loaded_ammo / 10;
		int loaded2 = loaded_ammo % 10;
		int backup1 = backup_ammo / 10;
		int backup2 = backup_ammo % 10;

		loaded1Name = loadedColor + std::to_string(loaded1);
		loaded2Name = loadedColor + std::to_string(loaded2);
		backup1Name = backupColor + std::to_string(backup1);
		backup2Name = backupColor + std::to_string(backup2);

		loaded1Sprite = RM.getSprite(loaded1Name);
		loaded2Sprite = RM.getSprite(loaded2Name);
		backup1Sprite = RM.getSprite(backup1Name);
		backup2Sprite = RM.getSprite(backup2Name);

		if (!loaded1Sprite) {
			LM.writeLog("Weapon: Warning! Sprite loaded1 '%s' not found", loaded1Name);
		}

		if (!loaded2Sprite) {
			LM.writeLog("Weapon: Warning! Sprite loaded2 '%s' not found", loaded2Name);
		}

		if (!backup1Sprite) {
			LM.writeLog("Weapon: Warning! Sprite backup1 '%s' not found", backup1Name);
		}

		if (!backup2Sprite) {
			LM.writeLog("Weapon: Warning! Sprite backup2 '%s' not found", backup2Name);
		}

		loaded_1->setSprite(loaded1Sprite);
		loaded_2->setSprite(loaded2Sprite);
		backup_1->setSprite(backup1Sprite);
		backup_2->setSprite(backup2Sprite);

		positionSprites();
	}
}

void AmmoDisplay::draw() {
	if (!NM.isServer()) {
		df::Object::draw();
		df::ObjectList hero_list = WM.objectsOfType("Hero");
		if ((!hero_list.isEmpty()) && (owner->getCurrentWeapon() != NULL)) {
			DM.drawString(getPosition() - df::Vector(0, -1 + getSprite()->getHeight() / 2), owner->getCurrentWeapon()->getWeaponName(), df::Justification::CENTER_JUSTIFIED, df::Color::CYAN);
		}
		loaded_1->draw();
		loaded_2->draw();
		backup_1->draw();
		backup_2->draw();
	}
}

void AmmoDisplay::positionSprites() {
	//setPosition(WM.getView().getCorner() + df::Vector(WM.getView().getHorizontal(), WM.getView().getVertical()) - df::Vector(getSprite()->getWidth() / 2, getSprite()->getHeight() / 2));
	setPosition(WM.getView().getCorner() + df::Vector(0, WM.getView().getVertical()) - df::Vector(-getSprite()->getWidth() / 2, getSprite()->getHeight() / 2));

	float digit_width = loaded_1->getSprite()->getWidth();
	float slash_width = slash->getSprite()->getWidth();

	//arrange the digits inside the ammo display view
	slash->setPosition(getPosition() + df::Vector(0, 0.5f));
	loaded_2->setPosition(slash->getPosition() - df::Vector(slash_width / 2, 0) - df::Vector(digit_width / 2, 0));
	loaded_1->setPosition(loaded_2->getPosition() - df::Vector(digit_width, 0));
	backup_1->setPosition(slash->getPosition() + df::Vector(slash_width / 2, 0) + df::Vector(digit_width / 2, 0));
	backup_2->setPosition(backup_1->getPosition() + df::Vector(digit_width, 0));
}