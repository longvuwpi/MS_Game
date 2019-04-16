#pragma once
#include "Button.h"
#include "Server.h"

class ReadyButton : public df::Button {

private:
	int last_ready_sock_in;
	df::Vector mouse_pos;
public:
	ReadyButton();
	~ReadyButton();
	void addReadyPlayer(int sock_in);
	void showButton();
	int eventHandler(const df::Event *p_e);
	void callback() override;
};
