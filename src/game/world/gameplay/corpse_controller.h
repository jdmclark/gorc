#pragma once

#include "thing_controller.h"

namespace gorc {
namespace game {
namespace world {
namespace gameplay {

class corpse_controller : public thing_controller {
public:
	using thing_controller::thing_controller;

	virtual void create_controller_data(int thing_id) override;
	virtual void remove_controller_data(int thing_id) override;
	virtual void handle_animation_marker(int thing_id, flags::key_marker_type marker) override;
};

}
}
}
}
