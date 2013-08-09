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

	virtual void Update(Id<Thing> thing_id, double dt) override;
	virtual void RemoveControllerData(Id<Thing> thing_id) override;
	virtual void CreateControllerData(Id<Thing> thing_id) override;
};

}
}
}
}
}
