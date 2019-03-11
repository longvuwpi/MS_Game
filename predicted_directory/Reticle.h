//
// Reticle.h
//
#pragma once
#include "Object.h"
#include "Hero.h"
#define RETICLE_CHAR '.'

class Hero;

class Reticle : public df::Object {

 private:
  int m_socket_index;  // On Server, socket index of connection.
  float expand_size;
  Hero *hero;
 public:

	 Reticle(Hero* owner);
	 void draw(void);
	 void expand();
	 int eventHandler(const df::Event *p_e);

  // Return socket index.
  //int getSocketIndex() const;

  //// Set socket index.
  //void setSocketIndex(int new_socket_index);

  //// Custom serialize for local attributes.
  //std::string serialize(bool all=false);

  //// Custom deserialize for local attributes.
  //int deserialize(std::string str);
};
