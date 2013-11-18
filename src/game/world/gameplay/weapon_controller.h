#pragma once

#include "thing_controller.h"

namespace gorc {
namespace game {
namespace world {
namespace gameplay {

class weapon_controller : public thing_controller {
public:
	using thing_controller::thing_controller;

	virtual void touched_thing(int thing_id, int touched_thing_id) override;
	virtual void touched_surface(int thing_id, int touched_surface_id) override;

	virtual void update(int thing_id, double dt) override;
};

}
}
}
}
