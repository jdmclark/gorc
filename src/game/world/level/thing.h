#pragma once

#include "content/assets/template.h"
#include "physicsobjectdata.h"
#include <btBulletDynamicsCommon.h>
#include <memory>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing : public Content::Assets::Template {
public:
	ThingObjectData ObjectData;
	std::unique_ptr<btDefaultMotionState> MotionState;
	std::unique_ptr<btRigidBody> RigidBody;

	Thing() = default;
	Thing(const Content::Assets::Template& tpl);

	const Content::Assets::Template& operator=(const Content::Assets::Template& tpl);
};

}
}
}
}
