//
// GameStart.cpp
//

// Engine includes.
#include "Color.h"
#include "EventKeyboard.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "Vector.h"

// Game includes.
#include "GameStart.h"
#include "Hero.h"
#include "Music.h"
#include "Points.h"
#include "Saucer.h"
#include "Platform.h"

GameStart::GameStart() {
  setType("GameStart");

  // Link to "message" sprite.
  df::Sprite *p_temp_sprite = RM.getSprite("gamestart");
  if (!p_temp_sprite)
    LM.writeLog("GameStart::GameStart(): Warning! Sprite 'gamestart' not found");
  else {
    setSprite(p_temp_sprite);
    setSpriteSlowdown(15);		  
  }

  // Put in center of screen.
  setLocation(df::CENTER_CENTER);

  // Register for "keyboard" event.
  registerInterest(df::KEYBOARD_EVENT);

  // Play start music.
  p_music = RM.getMusic("start music");
  playMusic();
}

// Play start music.
void GameStart::playMusic() {
  p_music->play();
}

// Handle event.
// Return 0 if ignored, else 1.
int GameStart::eventHandler(const df::Event *p_e) {

  if (p_e->getType() == df::KEYBOARD_EVENT) {
    df::EventKeyboard *p_keyboard_event = (df::EventKeyboard *) p_e;
    switch (p_keyboard_event->getKey()) {
    case df::Keyboard::P: 			// play
      start();
      break;
    case df::Keyboard::Q:			// quit
      GM.setGameOver();
      break;
    default:
      break;
    }
    return 1;
  }

  // If get here, have ignored this event.
  return 0;
}

void GameStart::start() {

  // Create hero.
  Hero* hero = new Hero;

  //printf("%f", WM.setViewFollowing((df::Object*)hero));

  // Create platforms to stand on
  Platform* platform = new Platform(df::Vector(7, 65));
  Platform* platform2 = new Platform(df::Vector(70, 55));
  Platform* platform3 = new Platform(df::Vector(133, 60));
  Platform* platform4 = new Platform(df::Vector(196, 47));
  Platform* platform5 = new Platform(df::Vector(263, 65));


  // setup world and view boundaries
  int world_horiz, world_vert;
  world_horiz = DM.getHorizontal()*10;
  world_vert = DM.getVertical();
  int window_horiz = DM.getHorizontal();
  df::Vector world_corner(0, 0);
  df::Box boundary(world_corner, (float)world_horiz, (float)world_vert);
  WM.setBoundary(boundary);
  df::Box window_boundary(world_corner, (float)window_horiz, (float)world_vert);
  WM.setView(window_boundary);
  //WM.setViewSlack(df::Vector(1 / 3.0f, 1 / 3.0f));

  // Setup heads-up display.
  new Points;		                     // Points display.

  // Pause start music.
  p_music->pause();

  // When game starts, become inactive.
  setActive(false);
}

// Override default draw so as not to display "value".
void GameStart::draw() {
  df::Object::draw();
}
