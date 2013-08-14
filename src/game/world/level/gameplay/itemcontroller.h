#pragma once

#include "thingcontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class ItemController : public ThingController {
public:
	using ThingController::ThingController;

	virtual void Update(int thing_id, double dt) override;
	virtual void CreateControllerData(int thing_id) override;
	virtual void Taken(int thing_id, int player_id) override;
};

}
}
}
}
}
