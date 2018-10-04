//
// Hero.h
//
#pragma once
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "Object.h"
#include "Reticle.h"
#include "Platform.h"
#include "ObjectList.h"
#include "ObjectListIterator.h"
#include "ViewObject.h"

class Hero : public df::Object {

 private:
  Reticle *p_reticle;
  Platform *p_OnPlatform;
  df::Sprite * walk_sprite;
  df::Sprite * duck_sprite;
  int fire_slowdown;
  int fire_countdown;
  int move_slowdown;
  int move_countdown;
  int nuke_count;
  bool isDucking;
  df::ObjectList weapon_list;
  df::ObjectListIterator *weapon_selector;
  df::ViewObject *weapon_view;
  void mouse(const df::EventMouse *p_mouse_event);
  void kbd(const df::EventKeyboard *p_keyboard_event);
  void move(int dx, int dy);
  void fire(df::Vector target);
  void step();
  void jump();
  void setWalkingSprite();
  void setDuckingSprite();
  
 public:
  Hero();
  ~Hero();
  int eventHandler(const df::Event *p_e);
};
