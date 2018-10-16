//
// Reticle.h
//
#pragma once
#include "Object.h"

#define RETICLE_CHAR '.'

class Reticle : public df::Object {
private:
	float expandSize;
 public:
  Reticle();
  void draw(void);
  void expand();
  int eventHandler(const df::Event *p_e);
};
