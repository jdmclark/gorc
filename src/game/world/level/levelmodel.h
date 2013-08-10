#pragma once

#include "content/assets/level.h"
#include "cog/instance.h"
#include "framework/pool.h"
#include "thing.h"
#include "surface.h"
#include "sounds/sound.h"
#include "physics/sectorbroadphasefilter.h"
#include "game/world/level/animations/animationmodel.h"
#include "game/world/level/scripts/scriptmodel.h"
#include "game/world/level/sounds/soundmodel.h"
#include "game/world/level/keys/keymodel.h"
#include <vector>

using namespace Gorc::Math;

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel {
public:
	std::unique_ptr<SectorBroadphaseFilter> BroadphaseFilter;
	const Content::Assets::Level& Level;
	Content::Assets::LevelHeader Header;
	std::vector<Content::Assets::LevelAdjoin> Adjoins;
	std::vector<Surface> Surfaces;
	std::vector<Content::Assets::LevelSector> Sectors;

	Animations::AnimationModel AnimationModel;
	Scripts::ScriptModel ScriptModel;
	Sounds::SoundModel SoundModel;
	Keys::KeyModel KeyModel;

	Pool<Thing> Things;

	int CameraThingId;
	unsigned int CameraSector;
	Vector<3> CameraPosition;
	Vector<3> CameraLook = Vec(0.0f, 1.0f, 0.0f);
	Vector<3> CameraUp = Vec(0.0f, 0.0f, 1.0f);
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

	LevelModel(Content::Manager& Manager, Cog::Compiler& CogCompiler, const Content::Assets::Level& Level);

	void UpdateSurfacePhysicsProperties(int surface, bool initial = false);
};

}
}
}
}
