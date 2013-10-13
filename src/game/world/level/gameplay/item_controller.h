#pragma once

#include "thing_controller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class ItemController : public ThingController {
public:
	using ThingController::ThingController;

	virtual void CreateControllerData(int thing_id) override;
	virtual void Taken(int thing_id, int player_id) override;
};

}
}
}
}
}
