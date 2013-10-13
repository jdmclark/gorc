#pragma once

#include "character_controller.h"
#include "game/world/level/physics/shape.h"
#include <vector>
#include <set>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

namespace Physics {
class ContactRangeConsumer;
}

namespace Gameplay {

class PlayerController : public CharacterController {
	void FindSectorRestingManifolds(const Physics::Sphere& sphere, int sector_id, std::set<int>& closed, std::vector<Math::Vector<3>>& manifolds);

public:
	using CharacterController::CharacterController;

	virtual void Update(int thing_id, double dt) override;
};

}
}
}
}
}
