#pragma once

#include "content/assets/template.h"
#include "gameplay/thingcontroller.h"
#include "physics/objectdata.h"
#include <memory>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

namespace Sounds {
class Sound;
}

namespace Keys {
class KeyMix;
class KeyState;
}

class Thing : public Content::Assets::Template {
public:
	Gameplay::ThingController* Controller;
	Physics::ThingObjectData ObjectData;

	bool PathMoving = false;
	int CurrentFrame = 0;
	int NextFrame = 0;
	int GoalFrame = 0;
	float PathMoveSpeed = 0.0f;

	int CurrentFoleyLoopChannel = -1;

	int AttachedSurface = -1;
	int AttachedThing = -1;
	Math::Vector<3> PrevAttachedThingPosition;

	int AttachedKeyMix = -1;
	int ActorWalkAnimation = -1;

	int CaptureCog = -1;
	float TimeAlive = 0.0f;

	Thing() = default;
	Thing(const Content::Assets::Template& tpl);
};

}
}
}
}
