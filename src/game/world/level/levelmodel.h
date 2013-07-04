#pragma once

#include "content/assets/level.h"
#include "cog/instance.h"
#include "framework/pool.h"
#include <btBulletDynamicsCommon.h>
#include <vector>

using namespace Gorc::Math;

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

enum class PhysicsCollideClass : unsigned int {
	Wall = 1,
	Adjoin = 2,
	Player = 4,
	Enemy = 8,
	Thing = 16
};

class PhysicsObjectData {
public:
	virtual ~PhysicsObjectData();
};

class SurfaceObjectData : public PhysicsObjectData {
public:
	unsigned int SectorId;
	unsigned int SurfaceId;
};

class ThingObjectData : public PhysicsObjectData {
public:
	unsigned int ThingId;
};

class Thing : public Content::Assets::Template {
public:
	ThingObjectData ObjectData;
	std::unique_ptr<btDefaultMotionState> MotionState;
	std::unique_ptr<btRigidBody> RigidBody;

	Thing() = default;
	Thing(const Content::Assets::Template& tpl);

	const Content::Assets::Template& operator=(const Content::Assets::Template& tpl);
};

class TimerEntity {
public:
	bool Expired = false;

	virtual ~TimerEntity();

	virtual void Update(double dt) = 0;
};

class SurfaceAnimEntity : public TimerEntity {
private:
	LevelModel& model;
	unsigned int surface;
	double framerate;
	FlagSet<Content::Assets::SurfaceAnimationFlag> flag;
	double framerate_accumulator = 0.0;
	unsigned int num_cels;

public:
	SurfaceAnimEntity(LevelModel& model, unsigned int Surface, double framerate, FlagSet<Content::Assets::SurfaceAnimationFlag> flag);

	void Update(double dt);
};

class LevelModel {
public:
	const Content::Assets::Level& Level;
	std::vector<int> MaterialCelNumber;
	std::vector<int> SurfaceCelNumber;
	std::vector<int> SurfaceAnimNumber;
	Pool<Thing> Things;
	Pool<std::unique_ptr<TimerEntity>> TimerEntities;
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
