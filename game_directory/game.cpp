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
	RM.loadSprite("sprites/level_won.txt", "level_won");
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

	//intro level
	RM.loadSprite("sprites/intro/1.txt", "intro_1");
	RM.loadSprite("sprites/intro/2.txt", "intro_2");
	RM.loadSprite("sprites/intro/3.txt", "intro_3");
	RM.loadSprite("sprites/intro/4.txt", "intro_4");
	RM.loadSprite("sprites/intro/5.txt", "intro_5");
	RM.loadSprite("sprites/intro/6.txt", "intro_6");
	RM.loadSprite("sprites/intro/7.txt", "intro_7");
	RM.loadSprite("sprites/intro/small.txt", "intro_small");

	//big numbers
	RM.loadSprite("sprites/number_font/green_0.txt", "green_0");
	RM.loadSprite("sprites/number_font/green_1.txt", "green_1");
	RM.loadSprite("sprites/number_font/green_2.txt", "green_2");
	RM.loadSprite("sprites/number_font/green_3.txt", "green_3");
	RM.loadSprite("sprites/number_font/green_4.txt", "green_4");
	RM.loadSprite("sprites/number_font/green_5.txt", "green_5");
	RM.loadSprite("sprites/number_font/green_6.txt", "green_6");
	RM.loadSprite("sprites/number_font/green_7.txt", "green_7");
	RM.loadSprite("sprites/number_font/green_8.txt", "green_8");
	RM.loadSprite("sprites/number_font/green_9.txt", "green_9");
	RM.loadSprite("sprites/number_font/red_0.txt", "red_0");
	RM.loadSprite("sprites/number_font/red_1.txt", "red_1");
	RM.loadSprite("sprites/number_font/red_2.txt", "red_2");
	RM.loadSprite("sprites/number_font/red_3.txt", "red_3");
	RM.loadSprite("sprites/number_font/red_4.txt", "red_4");
	RM.loadSprite("sprites/number_font/red_5.txt", "red_5");
	RM.loadSprite("sprites/number_font/red_6.txt", "red_6");
	RM.loadSprite("sprites/number_font/red_7.txt", "red_7");
	RM.loadSprite("sprites/number_font/red_8.txt", "red_8");
	RM.loadSprite("sprites/number_font/red_9.txt", "red_9");
	RM.loadSprite("sprites/number_font/yellow_0.txt", "yellow_0");
	RM.loadSprite("sprites/number_font/yellow_1.txt", "yellow_1");
	RM.loadSprite("sprites/number_font/yellow_2.txt", "yellow_2");
	RM.loadSprite("sprites/number_font/yellow_3.txt", "yellow_3");
	RM.loadSprite("sprites/number_font/yellow_4.txt", "yellow_4");
	RM.loadSprite("sprites/number_font/yellow_5.txt", "yellow_5");
	RM.loadSprite("sprites/number_font/yellow_6.txt", "yellow_6");
	RM.loadSprite("sprites/number_font/yellow_7.txt", "yellow_7");
	RM.loadSprite("sprites/number_font/yellow_8.txt", "yellow_8");
	RM.loadSprite("sprites/number_font/yellow_9.txt", "yellow_9");
	RM.loadSprite("sprites/number_font/slash.txt", "slash");

	//Ammo display
	RM.loadSprite("sprites/ammo_display.txt", "ammo_display");

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
	RM.loadSound("sounds/saucer_death.wav", "saucer_death");
	RM.loadSound("sounds/game-over.wav", "game over");
	RM.loadSound("sounds/level_won.wav", "level_won");
	

	//music
	RM.loadMusic("sounds/start-music.wav", "start music");	
	//level music
	RM.loadMusic("sounds/music_theme.wav", "music_theme");
	RM.loadMusic("sounds/music_boss.wav", "music_boss");
}

// Populate world with some objects.
void populateWorld(void) {

	// Spawn some Stars.
	for (int i = 0; i < 16; i++)
		new Star;

	// Create GameStart object.
	new GameStart();
}
