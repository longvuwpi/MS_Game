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
#include "EventMouse.h"
#include "EventStep.h"
#include "util.h"

ReadyButton::ReadyButton() {
	setType("ReadyButton");

	last_ready_sock_in = -1;

	setViewString("Ready");
	setColor(df::COLOR_DEFAULT);
	setHighlightColor(df::RED);
	setDefaultColor(df::COLOR_DEFAULT);
	setBorder(true);
	setPosition(df::Vector(WM.getView().getHorizontal() / 2, WM.getView().getVertical() / 2));

	// Buttons respond to mouse movement (for highlight) and click.
		registerInterest(df::NETWORK_MSE_EVENT);
		registerInterest(df::MSE_EVENT);
		registerInterest(df::STEP_EVENT);

	mouse_pos = df::Vector(0, 0);
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
				addReadyPlayer(e->getSocketIndex());
			}

			return 1;
		}
	}

	if ((p_e->getType() == df::MSE_EVENT) && (!NM.isServer()))
		{
		if (mouseInWindow()) {
			const df::EventMouse *p_mouse_event = dynamic_cast <const df::EventMouse *> (p_e);
			if (p_mouse_event->getMouseAction() == df::MOVED) {
				mouse_pos = p_mouse_event->getMousePosition();
				//return 1;
			}
		}
	}

	if ((p_e->getType() == df::STEP_EVENT) && (!NM.isServer())) {
		WM.setViewPosition(df::Vector(0, 0));
		//LM.writeLog("Drawing cursor");
		DM.drawCh(mouse_pos + df::Vector(0, -0.2f), '.', df::GREEN);
		DM.drawCh(mouse_pos + df::Vector(0, -0.7), '|', df::GREEN);
		DM.drawCh(mouse_pos + df::Vector(0, 0.9), '|', df::GREEN);
		DM.drawCh(mouse_pos + df::Vector(-1.4f, -0.3f), '_', df::GREEN);
		DM.drawCh(mouse_pos + df::Vector(1.4f, -0.3f), '_', df::GREEN);

		//return 1;
	}



	// If get here, have ignored this event.
	return df::Button::eventHandler(p_e);

}

void ReadyButton::addReadyPlayer(int sock_in) {
	//if (last_ready_sock_in == -1) {
	//	last_ready_sock_in = sock_in;
	//}
	//else {
	//	if (last_ready_sock_in != sock_in) {
	//		df::ObjectList ol = WM.objectsOfType("Server");
	//		df::ObjectListIterator oli(&ol);
	//		oli.first();
	//		Server *server = (Server *)oli.currentObject();
	//		server->startGame();
	//		WM.markForDelete(this);
	//	}
	//}
	df::ObjectList ol = WM.objectsOfType("Server");
	df::ObjectListIterator oli(&ol);
	oli.first();
	Server *server = (Server *)oli.currentObject();
	server->startGame();
	setActive(false);
}

void ReadyButton::callback() {
	setActive(false);
}

void ReadyButton::showButton() {
	setActive(true);
	last_ready_sock_in = -1;
}