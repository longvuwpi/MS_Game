//
// Reticle.h
//
#pragma once

#include "Hero.h"

#include "Object.h"

#define RETICLE_CHAR '.'

class Hero;

class Reticle : public df::Object {
private:
	float expand_size;
	Hero *hero;
 public:
  Reticle(Hero* owner);
  void draw(void);
  void expand();
  int eventHandler(const df::Event *p_e);
};
