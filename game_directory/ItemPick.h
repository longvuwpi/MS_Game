#pragma once
#include "Object.h"
#include "Sprite.h"

enum ItemType {AMMOREFILL, HEALTHPICK, POWERUP};

class ItemPick : public df::Object {
private:
    std::string item_name;
    ItemType item_type;
public:
    ItemPick(std::string ItemName, ItemType itemType);
    std::string getItemName();
	int eventHandler(const df::Event *p_e);
	void draw();
    ItemType getItemType();
};