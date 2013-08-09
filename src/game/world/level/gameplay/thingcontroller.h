#pragma once

#include "framework/math/vector.h"
#include "framework/id.h"
#include "content/flags/keymarkertype.h"

namespace Gorc {

namespace Content {
namespace Assets {
class Template;
}
}

namespace Game {
namespace World {
namespace Level {

class LevelPresenter;
class Thing;

namespace Gameplay {

class ThingController {
protected:
	LevelPresenter& presenter;

public:
	ThingController(LevelPresenter& presenter);
	virtual ~ThingController();

	virtual void Update(Id<Thing> thing_id, double dt) = 0;
	virtual void RemoveControllerData(Id<Thing> thing_id) = 0;
	virtual void CreateControllerData(Id<Thing> thing_id) = 0;
	virtual void HandleAnimationMarker(Id<Thing> thing_id, Flags::KeyMarkerType marker);
};

}
}
}
}
}
