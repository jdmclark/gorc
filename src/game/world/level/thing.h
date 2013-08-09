#pragma once

#include "content/assets/template.h"
#include "physics/physicsobjectdata.h"
#include "gameplay/thingcontroller.h"
#include <btBulletDynamicsCommon.h>
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
	ThingObjectData ObjectData;
	std::unique_ptr<btDefaultMotionState> MotionState;
	std::unique_ptr<btRigidBody> RigidBody;

	bool PathMoving = false;
	int CurrentFrame = 0;
	int NextFrame = 0;
	int GoalFrame = 0;
	float PathMoveSpeed = 0.0f;

	Id<Sounds::Sound> CurrentFoleyLoopChannel;

	unsigned int AttachedSurface = -1;
	Id<Thing> AttachedThing;
	Math::Vector<3> PrevAttachedThingPosition;

	// HACK: Play idle animation
	Id<Keys::KeyMix> AttachedKeyMix;
	Id<Keys::KeyState> ActorWalkAnimation;

	std::unique_ptr<btCollisionShape> ActorCollideShape;

	Thing() = default;
	Thing(const Content::Assets::Template& tpl);

	const Content::Assets::Template& operator=(const Content::Assets::Template& tpl);
};

}
}
}
}
