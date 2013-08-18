#pragma once

#include "thingcontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class CorpseController : public ThingController {
public:
	using ThingController::ThingController;

	virtual void CreateControllerData(int thing_id) override;
	virtual void RemoveControllerData(int thing_id) override;
	virtual void HandleAnimationMarker(int thing_id, Flags::KeyMarkerType marker) override;
};

}
}
}
}
}
