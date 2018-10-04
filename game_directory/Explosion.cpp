//
// Explosion.cpp
//

// Engine includes.
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "DisplayManager.h"

// Game includes.
#include "Explosion.h"

Explosion::Explosion(std::string sprite, float explRadius) {
  registerInterest(df::STEP_EVENT);

  // Link to "explosion" sprite.
  df::Sprite *p_temp_sprite = RM.getSprite(sprite);
  if (!p_temp_sprite)
    LM.writeLog("Explosion::Explosion(): Warning! Sprite '%s' not found", sprite);
  else
    setSprite(p_temp_sprite);

  setType("Explosion");

  time_to_live =  getSprite()->getFrameCount();
  radius = explRadius;
  setSolidness(df::SPECTRAL);
}

// Handle event.
// Return 0 if ignored, else 1.
int Explosion::eventHandler(const df::Event *p_e) {

  if (p_e->getType() == df::STEP_EVENT) {
    step();
    return 1;
  }

  // If get here, have ignored this event.
  return 0;
}

// Count down until explosion finished.
void Explosion::step() {
  time_to_live--;
  if (time_to_live <= 0)
    WM.markForDelete(this);
}

//If the explosion has an area of effect (radius > 0) then draw the area indicator
void Explosion::draw() {
	if (radius > 0) {
		int frameCount = getSprite()->getFrameCount();
		float scale = radius / frameCount;
		int currentRadius = (frameCount - time_to_live) * scale;
		bool draw = true;
		for (int i = -currentRadius; i <= currentRadius; i++) {
			for (int j = -currentRadius; j <= currentRadius; j++) {
				df::Vector at(i, j);
				int atRadius = at.getMagnitude();

				if (((i == j) || (i == -j) || (i == 0) || (j == 0)) && (atRadius == (currentRadius/2))) {
					for (int m = -(currentRadius / 2); m <= (currentRadius / 2); m++) {
						for (int n = -(currentRadius / 2); n <= (currentRadius / 2); n++) {
							df::Vector at2(m, n);
							int at2radius = (at2 - at).getMagnitude();
							if (at2radius <= ((currentRadius / 5))) {
								DM.drawCh(getPosition() + at + at2, '*', df::YELLOW);
							}
						}
					}

				}

				//if ((atRadius <= radius) && (atRadius >= (currentRadius/2))) {
				//		DM.drawCh(getPosition() + at, '*', df::YELLOW);
				//		//draw = !draw;
				//}
			}
		}
	}
	df::Object::draw();

}
