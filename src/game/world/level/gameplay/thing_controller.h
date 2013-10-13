#pragma once

#include "framework/math/vector.h"
#include "content/flags/key_marker_type.h"

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
	virtual ~ThingController() = 0;

	virtual void Update(int thing_id, double dt);
	virtual void RemoveControllerData(int thing_id);
	virtual void CreateControllerData(int thing_id);
	virtual void HandleAnimationMarker(int thing_id, Flags::KeyMarkerType marker);

	virtual void Taken(int thing_id, int player_id);
	virtual void TouchedThing(int thing_id, int touched_thing_id);
	virtual void TouchedSurface(int thing_id, int touched_surface_id);
};

}
}
}
}
}
