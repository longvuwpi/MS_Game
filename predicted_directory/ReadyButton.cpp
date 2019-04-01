#include "ReadyButton.h"

#include "DisplayManager.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "EventStep.h"
#include "EventOut.h"
#include "WorldManager.h"
#include "NetworkManager.h"
#include "Server.h"
#include "ViewObject.h"
#include "EventMouseNetwork.h"

ReadyButton::ReadyButton(Server * newserver) {
	server = newserver;
	last_ready_sock_in = -1;
	df::Sprite *p_temp_sprite = RM.getSprite("bullet_trail");
	if (!p_temp_sprite)
		LM.writeLog("Explosion::Explosion(): Warning! Sprite '%s' not found", "bullet_trail");
	else
		setSprite(p_temp_sprite);

	setLocation(df::CENTER_CENTER);
	setViewString("Ready");
	setColor(df::COLOR_DEFAULT);
	setHighlightColor(df::RED);
	setDefaultColor(df::COLOR_DEFAULT);
	setBorder(true);

	// Buttons respond to mouse movement (for highlight) and click.
	registerInterest(df::NETWORK_MSE_EVENT);

}


ReadyButton::~ReadyButton() {
	if (NM.isServer()) {
		df::ObjectList ol = WM.objectsOfType("Server");
		df::ObjectListIterator oli(&ol);
		oli.first();
		Server *server = (Server *)oli.currentObject();
		server->sendMessage(df::DELETE_OBJECT, this);
	}
}

int ReadyButton::eventHandler(const df::Event *p_e) {


	if (NM.isServer()) {
		if (p_e->getType() == df::NETWORK_MSE_EVENT) {
			df::EventMouseNetwork *e = (df::EventMouseNetwork *)p_e;
			df::EventMouse e_m;
			e_m.setMouseAction(e->getMouseAction());
			e_m.setMouseButton(e->getMouseButton());
			e_m.setMousePosition(e->getMousePosition());
			
			if ((mouseOverButton(&e_m)) && (e_m.getMouseAction() == df::CLICKED))		
			{
				if ((last_ready_sock_in != -1) && (last_ready_sock_in != e->getSocketIndex())) {
					//server->startLevel();
				}
			}

			return df::Button::eventHandler(&e_m);
		}
	}

	// If get here, have ignored this event.
	return 0;

}

void ReadyButton::callback() {

}