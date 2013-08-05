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

	int CurrentFoleyLoopChannel = -1;

	// HACK: Play idle animation
	int AttachedKeyMix = -1;

	std::unique_ptr<btCollisionShape> ActorCollideShape;

	Thing() = default;
	Thing(const Content::Assets::Template& tpl);

	const Content::Assets::Template& operator=(const Content::Assets::Template& tpl);
};

}
}
}
}
