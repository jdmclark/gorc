#pragma once

#include "thing_controller.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace gameplay {

class weapon_controller : public thing_controller {
public:
	using thing_controller::thing_controller;

	virtual void create_controller_data(int thing_id) override;

	virtual void touched_thing(int thing_id, int touched_thing_id) override;
	virtual void touched_surface(int thing_id, int touched_surface_id) override;
};

}
}
}
}
}
