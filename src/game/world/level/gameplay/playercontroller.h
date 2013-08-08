#pragma once

#include "charactercontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class PlayerController : public CharacterController {
public:
	using CharacterController::CharacterController;

	virtual void Update(unsigned int thing_id, double dt) override;
};

}
}
}
}
}
