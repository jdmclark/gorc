#pragma once

#include "content/assets/template.h"
#include "gameplay/thingcontroller.h"
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

	inline void Reset() {
		Controller = nullptr;

		PathMoving = false;
		CurrentFrame = 0;
		NextFrame = 0;
		GoalFrame = 0;
		PathMoveSpeed = 0.0f;

		CurrentFoleyLoopChannel = -1;

		AttachedSurface = -1;
		AttachedThing = -1;
		PrevAttachedThingPosition = Math::Zero<3>();

		AttachedKeyMix = -1;
		ActorWalkAnimation = -1;

		CaptureCog = -1;
	}

	const Content::Assets::Template& operator=(const Content::Assets::Template& tpl);
};

}
}
}
}
