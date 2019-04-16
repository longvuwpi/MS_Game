//
// Server.h
//

#ifndef SERVER_H
#define SERVER_H

// Engine includes.
#include "NetworkNode.h"
#include "GameStart.h"
#include "Hero.h"

const int MAX_CONNECTIONS = 3;  // Maximum number of Clients.

class Server : public df::NetworkNode {

private:
	bool is_predicted;
	// If any Objects need to be synchronized, send to client.
	void doSync();
	GameStart *game_start;
	Hero * newHero1;
	Hero * newHero2;
public:
	Server(bool is_predicted);

	// Handle event.
	// Return 0 if ignored, else 1.
	int eventHandler(const df::Event *p_e);

	// Handle accept.
	int handleAccept(const df::EventNetwork *p_e);
	bool getIsPredicted();

	void startGame();
	void introScene();
};

#endif
