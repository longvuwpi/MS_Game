//
// EventNuke.h
//

#include "Object.h"

const std::string NUKE_EVENT =  "nuke";

class Platform : public df::Object {

 public:
	 Platform();
	 int eventHandler(const df::Event *p_e);
};
