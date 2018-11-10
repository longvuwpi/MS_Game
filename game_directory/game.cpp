//
// game.cpp
// 

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "Pause.h"
#include "ResourceManager.h"
#include "Fraps.h"

// Game includes.
#include "GameStart.h"
#include "Star.h"

// Function prototypes.
void loadResources(void);
void populateWorld(void);

int main(int argc, char *argv[]) {

	// Start up game manager.
	if (GM.startUp()) {
		LM.writeLog("Error starting game manager!");
		GM.shutDown();
		return 0;
	}

	// Set flush of logfile during development (when done, make false).
	LM.setFlush(true);

	// Show splash screen.
	df::splash();

	// Load game resources.
	loadResources();
	df::Fraps *fraps = new df::Fraps();
	fraps->setLocation(df::TOP_CENTER);
	// Populate game world with some objects.
	populateWorld();

	// Enable player to pause game.
	new df::Pause(df::Keyboard::F10);

	// Run game (this blocks until game loop is over).
	GM.run();

	// Shut everything down.
	GM.shutDown();
}

// Load resources (sprites, sound effects, music).
void loadResources(void) {
	//UI
	RM.loadSprite("sprites/level_select.txt", "level_select");
	RM.loadSprite("sprites/gamestart-spr.txt", "gamestart");
	RM.loadSprite("sprites/gameover-spr.txt", "gameover");
	RM.loadSprite("sprites/level_intro.txt", "level_intro");
	RM.loadSprite("sprites/level_1.txt", "level_1");

	RM.loadSprite("sprites/saucer-spr.txt", "saucer");
	RM.loadSprite("sprites/ship-spr.txt", "ship");
	RM.loadSprite("sprites/boss.txt", "boss");
	RM.loadSprite("sprites/boss-weakpoint.txt", "boss-weakpoint");

	//guns
	//RM.loadSprite("sprites/ak47.txt", "AK47");
	RM.loadSprite("sprites/ak47_2.txt", "AK47");
	RM.loadSprite("sprites/awp.txt", "AWP");
	RM.loadSprite("sprites/grenade-launcher.txt", "GrenadeLauncher");

	//bullets
	RM.loadSprite("sprites/ak47_bullet.txt", "AK47_bullet");
	RM.loadSprite("sprites/awp_bullet.txt", "AWP_bullet");
	RM.loadSprite("sprites/grenade_bullet.txt", "GrenadeLauncher_bullet");
	RM.loadSprite("sprites/bullet_trail.txt", "bullet_trail");

	RM.loadSprite("sprites/explosion-spr.txt", "explosion");
	RM.loadSprite("sprites/nuke.txt", "nuke");
	//RM.loadSprite("sprites/character-walk.txt", "character-walk");
	RM.loadSprite("sprites/character-walk_2.txt", "character-walk");
	RM.loadSprite("sprites/character-duck.txt", "character-duck");
	RM.loadSprite("sprites/platform.txt", "platform");
	RM.loadSprite("sprites/refill-ammo.txt", "ammo-refill");

	//Fire sounds
	RM.loadSound("sounds/ak47_fire.wav", "AK47_fire");
	RM.loadSound("sounds/awp_fire.wav", "AWP_fire");
	RM.loadSound("sounds/grenade_fire.wav", "GrenadeLauncher_fire");
	//ammo sounds
	RM.loadSound("sounds/ammo_empty.wav", "ammo_empty");
	RM.loadSound("sounds/ak47_reload.wav", "AK47_reload");
	RM.loadSound("sounds/awp_reload.wav", "AWP_reload");
	RM.loadSound("sounds/launcher_reload.wav", "GrenadeLauncher_reload");

	//AWP bolt
	RM.loadSound("sounds/awp_bolt.wav", "awp_bolt");

	//pickup sounds
	RM.loadSound("sounds/pickup1.wav", "pickup1");
	RM.loadSound("sounds/pickup2.wav", "pickup2");

	RM.loadSound("sounds/explode.wav", "explode");
	RM.loadSound("sounds/nuke.wav", "nuke");
	RM.loadSound("sounds/game-over.wav", "game over");
	RM.loadMusic("sounds/start-music.wav", "start music");
}

// Populate world with some objects.
void populateWorld(void) {

	// Spawn some Stars.
	for (int i = 0; i < 16; i++)
		new Star;

	// Create GameStart object.
	new GameStart();
}
