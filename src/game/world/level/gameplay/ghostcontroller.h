#pragma once

#include "thingcontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class GhostController : public ThingController {
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
