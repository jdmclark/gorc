#pragma once

#include "thing_controller.h"

namespace gorc {
namespace game {
namespace world {
namespace level {

class thing;

namespace gameplay {

class cog_controller : public thing_controller {
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
