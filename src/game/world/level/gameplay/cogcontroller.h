#pragma once

#include "thingcontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing;

namespace Gameplay {

class CogController : public ThingController {
private:
	void UpdateThingPathMoving(Id<Thing> thing_id, Thing& thing, double dt);

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
