#pragma once

#include "framework/math/vector.h"
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

namespace Gameplay {

class ThingController {
protected:
	LevelPresenter& presenter;

public:
	ThingController(LevelPresenter& presenter);
	virtual ~ThingController();

	virtual void Update(unsigned int thing_id, double dt) = 0;
	virtual void RemoveControllerData(unsigned int thing_id) = 0;
	virtual void CreateControllerData(unsigned int thing_id) = 0;
	virtual void HandleAnimationMarker(unsigned int thing_id, Flags::KeyMarkerType marker);
};

}
}
}
}
}
