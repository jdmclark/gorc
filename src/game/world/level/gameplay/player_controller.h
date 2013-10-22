#pragma once

#include "character_controller.h"
#include "game/world/level/physics/shape.h"
#include <vector>
#include <set>

namespace gorc {
namespace game {
namespace world {
namespace level {

namespace physics {
class contact_range_consumer;
}

namespace gameplay {

class player_controller : public character_controller {
	void find_sector_resting_manifolds(const physics::sphere& sphere, int sector_id, std::set<int>& closed, std::vector<vector<3>>& manifolds,
			const vector<3>& vel_dir);
	void find_thing_resting_manifolds(const physics::sphere& sphere, std::set<int>& closed, std::vector<vector<3>>& manifolds,
			const vector<3>& vel_dir, int current_thing_id);

	void step_physics(int thing_id, double dt);

public:
	using character_controller::character_controller;

	virtual void update(int thing_id, double dt) override;
};

}
}
}
}
}
