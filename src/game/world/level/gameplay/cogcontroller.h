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
	void UpdateThingPathMoving(unsigned int thing_id, Thing& thing, double dt);

public:
	using ThingController::ThingController;

	virtual void Update(unsigned int thing_id, double dt) override;
	virtual void RemoveControllerData(unsigned int thing_id) override;
	virtual void CreateControllerData(unsigned int thing_id) override;
};

}
}
}
}
}
