#pragma once
#include "Button.h"
#include "Server.h"

class ReadyButton : public df::Button {

private:
	Server * server;
	int last_ready_sock_in;
public:
	ReadyButton(Server* server);
	~ReadyButton();

	int eventHandler(const df::Event *p_e);
	void callback() override;
};
