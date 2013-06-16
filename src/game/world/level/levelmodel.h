#pragma once

#include "content/assets/level.h"
#include <btBulletDynamicsCommon.h>
#include <vector>

using namespace Gorc::Math;

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

enum class PhysicsCollideClass : unsigned int {
	Wall = 1,
	Adjoin = 2,
	Player = 4
};

class PhysicsObjectData {
public:
	virtual ~PhysicsObjectData();
};

class CameraObjectData : public PhysicsObjectData {
};

class SurfaceObjectData : public PhysicsObjectData {
public:
	size_t SectorId;
	size_t SurfaceId;
};

class LevelModel {
public:
	const Content::Assets::Level& Level;

	Vector<3> CameraPosition;
	Vector<3> CameraLook = Vec(0.0f, 1.0f, 0.0f);
	Vector<3> CameraUp = Vec(0.0f, 0.0f, 1.0f);
	static constexpr float CameraRadius = 0.06f;
	size_t CameraCurrentSector = 104;
	Vector<3> CameraVelocity = Zero<3>();

	btDbvtBroadphase Broadphase;
	btDefaultCollisionConfiguration CollisionConfiguration;
	btCollisionDispatcher Dispatcher;
	btSequentialImpulseConstraintSolver ConstraintSolver;
	btDiscreteDynamicsWorld DynamicsWorld;

	std::vector<int> SurfaceIndexBuffer;
	std::vector<float> SurfaceVertexBuffer;

	std::vector<std::unique_ptr<btTriangleIndexVertexArray>> SurfaceIndexVertexArrays;
	std::vector<std::unique_ptr<btBvhTriangleMeshShape>> SurfaceShapes;
	std::vector<std::unique_ptr<btRigidBody>> SurfaceRigidBodies;
	btDefaultMotionState SurfaceMotionState;
	std::vector<SurfaceObjectData> SurfaceObjectData;

	btSphereShape CameraShape;
	btDefaultMotionState CameraMotionState;
	std::unique_ptr<btRigidBody> CameraRigidBody;
	CameraObjectData CameraObjectData;

	LevelModel(const Content::Assets::Level& Level);
};

}
}
}
}
