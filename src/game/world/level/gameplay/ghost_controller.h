#pragma once

#include "thing_controller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class GhostController : public ThingController {
public:
	using ThingController::ThingController;

	virtual void CreateControllerData(int thing_id) override;
	virtual void RemoveControllerData(int thing_id) override;
};

}
}
}
}
}
