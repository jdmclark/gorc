#pragma once

#include "content/assets/level.h"
#include "cog/instance.h"
#include "framework/pool.h"
#include "thing.h"
#include "animation.h"
#include <vector>

using namespace Gorc::Math;

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel {
public:
	const Content::Assets::Level& Level;
	std::vector<int> MaterialCelNumber;
	std::vector<int> SurfaceCelNumber;
	std::vector<int> SurfaceAnimNumber;
	Pool<Thing> Things;
	Pool<std::unique_ptr<Animation>> Animations;
	std::vector<std::unique_ptr<Cog::Instance>> Cogs;

	Vector<3> CameraLook = Vec(0.0f, 1.0f, 0.0f);
	Vector<3> CameraUp = Vec(0.0f, 0.0f, 1.0f);
	static constexpr float CameraRadius = 0.06f;
	Vector<3> CameraVelocity = Zero<3>();

	std::vector<Content::Assets::Template const*> SpawnPoints;
	unsigned int CurrentSpawnPoint = 0;

	btDbvtBroadphase Broadphase;
	btDefaultCollisionConfiguration CollisionConfiguration;
	btCollisionDispatcher Dispatcher;
	btSequentialImpulseConstraintSolver ConstraintSolver;
	btDiscreteDynamicsWorld DynamicsWorld;

	std::vector<std::unique_ptr<btRigidBody>> SurfaceRigidBodies;
	btDefaultMotionState SurfaceMotionState;
	std::vector<SurfaceObjectData> SurfaceObjectData;

	std::unique_ptr<btSphereShape> CameraShape;
	unsigned int CameraThingId;

	LevelModel(const Content::Assets::Level& Level);

	unsigned int CreateThing(const Content::Assets::Template& tpl, unsigned int sector_num, const Math::Vector<3>& pos, const Math::Vector<3>& orientation);
	unsigned int CreateThing(const std::string& tpl_name, unsigned int sector_num, const Math::Vector<3>& pos, const Math::Vector<3>& orientation);
};

}
}
}
}
