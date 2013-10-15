#pragma once

#include "thing_controller.h"

namespace gorc {
namespace game {
namespace world {
namespace level {

class thing;

namespace gameplay {

class cog_controller : public thing_controller {
private:
	void update_thing_path_moving(int thing_id, thing& thing, double dt);

public:
	using thing_controller::thing_controller;

	virtual void update(int thing_id, double dt) override;
	virtual void create_controller_data(int thing_id) override;
};

}
}
}
}
}
