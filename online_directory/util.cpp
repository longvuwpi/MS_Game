//
// util.cpp
//

// Engine includes.
#include "Config.h"
#include "DisplayManager.h"

// Game includes.
#include "util.h"

// Return true if mouse inside game window, else false.
bool mouseInWindow() {
  sf::Vector2i lp = sf::Mouse::getPosition(*DM.getWindow());
  if (lp.x < df::Config::getInstance().getWindowHorizontalPixels() &&
      lp.x > 0 &&
      lp.y < df::Config::getInstance().getWindowVerticalPixels() &&
      lp.y > 0) {
    return true;
  }
  return false;
}
