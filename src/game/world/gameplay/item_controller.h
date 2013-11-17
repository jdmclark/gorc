#pragma once

#include "thing_controller.h"

namespace gorc {
namespace game {
namespace world {
namespace gameplay {

class item_controller : public thing_controller {
public:
	using thing_controller::thing_controller;

	virtual void taken(int thing_id, int player_id) override;
};

}
}
}
}
