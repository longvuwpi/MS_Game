//
// game.cpp - Hello, Network!
// 

#define VERSION 1.2

// System includes.
#include <stdlib.h>		// for setenv()
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <time.h>
#if defined(_WIN32) || defined(_WIN64)
#include <process.h>
#else
#include <sys/types.h>		// for getpid()
#include <unistd.h>		// for getpid()
#endif

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"

// Game includes.
#include "Client.h"
#include "Server.h"

// Function prototypes.
void usage(void);
void loadResources(void);
 
int main(int argc, char *argv[]) {
  bool is_server;
  bool is_predicted;

  auto current_time = std::chrono::system_clock::now();
  std::time_t current_time_t = std::chrono::system_clock::to_time_t(current_time);
  std::to_string(current_time_t);
  struct tm * timeinfo = localtime(&current_time_t);
  char timestring[80];
  //strftime(timestring, 80, "%H%M%S", timeinfo);
  strftime(timestring, 80, "%H%M", timeinfo);

  // Must specify 0 args if server, 1 arg if client.
  if (argc != 2 && argc != 3)
    usage();

  std::string log_file;

  // Setup logfile: server or client.
  if (argc == 2) {
    is_server = true;
	std::string predicted_string;
	//if (strcmp("true", argv[1])) {
	if (std::string(argv[1]).find("true") != std::string::npos) {
		is_predicted = true;
		predicted_string = "_server_p.log";
	}
	else {
		is_predicted = false;
		predicted_string = "_server_np.log";
	}

	 log_file = timestring + predicted_string;

#if defined(_WIN32) || defined(_WIN64)
	_putenv_s("DRAGONFLY_LOG", log_file.c_str());
#else
	setenv("DRAGONFLY_LOG", log_file.c_str(), 1);
#endif
  } else {
    is_server = false;
#if defined(_WIN32) || defined(_WIN64)
    int pid = _getpid();
#else
    int pid = getpid();
#endif    
    log_file = std::string(timestring) + "_client_" + std::string(argv[2]) + ".log";
    //log_file += std::to_string(pid);
    //log_file += ".log";
#if defined(_WIN32) || defined(_WIN64)
    _putenv_s("DRAGONFLY_LOG", log_file.c_str());
#else
    setenv("DRAGONFLY_LOG", log_file.c_str(), 1);
#endif
  }

  // Start up game manager.
  if (GM.startUp())  {
    LM.writeLog("Error starting game manager!");
    GM.shutDown();
    return 0;
  }

  // Setup logging.
  LM.setFlush(true);
  //LM.setLogLevel(20);
  LM.setLogLevel(1);
  LM.writeLog("Hello, Network! (v%.1f)", VERSION);
  LM.writeLog(log_file.c_str());

  // Load game resources.
  loadResources();

  // Startup Server/Client.
  df::NetworkNode *p_node;
  if (is_server) {
    df::splash();
    p_node = (df::NetworkNode *) new Server(is_predicted);
  } else {
    p_node = (df::NetworkNode *) new Client(argv[1]);
  }

  // Run game (this blocks until game loop is over).
  GM.run();

  // Tell any connected games to also shut down.
  p_node -> sendMessage(df::SET_GAME_OVER);
  
  // Shut everything down.
  GM.shutDown();
}

///////////////////////////////////////////////
// Load resources (sprites, sound effects, or music).
void loadResources(void) {
  RM.loadSprite("sprites/explosion-spr.txt", "explosion");

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

///////////////////////////////////////////////
// Print usage message and exit.
void usage(void) {
  fprintf(stderr, "Hello, Network! (v%.1f)\n", VERSION);
  fprintf(stderr, "Usage: game.exe [server name]:\n");
  fprintf(stderr, "\t[server name] [username]  run as a client, connect to server\n");
  fprintf(stderr, "\t[is_predicted]			   otherwise, run as a sever\n");
  exit(1);
}
