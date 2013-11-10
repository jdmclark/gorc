#pragma once

#include "thing_controller.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace gameplay {

class ghost_controller : public thing_controller {
public:
	using thing_controller::thing_controller;

	virtual void create_controller_data(int thing_id) override;
	virtual void remove_controller_data(int thing_id) override;
};

}
}
}
}
}
