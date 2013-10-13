#pragma once

#include "thing_controller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class WeaponController : public ThingController {
public:
	using ThingController::ThingController;

	virtual void CreateControllerData(int thing_id) override;

	virtual void TouchedThing(int thing_id, int touched_thing_id) override;
	virtual void TouchedSurface(int thing_id, int touched_surface_id) override;
};

}
}
}
}
}
