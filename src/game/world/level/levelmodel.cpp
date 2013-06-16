#include "levelmodel.h"

#include "framework/flagset.h"

Gorc::Game::World::Level::PhysicsObjectData::~PhysicsObjectData() {
	return;
}

Gorc::Game::World::Level::LevelModel::LevelModel(const Gorc::Content::Assets::Level& Level)
	: Level(Level), Dispatcher(&CollisionConfiguration), DynamicsWorld(&Dispatcher, &Broadphase, &ConstraintSolver, &CollisionConfiguration),
	  SurfaceMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0))), SurfaceObjectData(Level.Surfaces.size()),
	  CameraShape(CameraRadius), CameraMotionState(btTransform(btQuaternion(0,0,0,1), BtVec(Level.Sectors[CameraCurrentSector].Center))) {
	DynamicsWorld.setGravity(btVector3(0, 0, -Level.Header.WorldGravity));

	// Create level index and vertex buffers.
	for(const auto& surf : Level.Surfaces) {
		const auto& first_vx = std::get<0>(surf.Vertices[0]);
		for(size_t j = 2; j < surf.Vertices.size(); ++j) {
			SurfaceIndexBuffer.push_back(first_vx);
			SurfaceIndexBuffer.push_back(std::get<0>(surf.Vertices[j - 1]));
			SurfaceIndexBuffer.push_back(std::get<0>(surf.Vertices[j]));
		}
	}

	for(const auto& vert : Level.Vertices) {
		SurfaceVertexBuffer.push_back(Get<X>(vert));
		SurfaceVertexBuffer.push_back(Get<Y>(vert));
		SurfaceVertexBuffer.push_back(Get<Z>(vert));
	}

	size_t i = 0;
	for(const auto& surf : Level.Surfaces) {
		SurfaceIndexVertexArrays.emplace_back(new btTriangleIndexVertexArray(
				surf.Vertices.size() - 2, &SurfaceIndexBuffer[i], sizeof(int) * 3,
				SurfaceVertexBuffer.size() / 3, &SurfaceVertexBuffer[0], sizeof(float) * 3));
		SurfaceShapes.emplace_back(new btBvhTriangleMeshShape(SurfaceIndexVertexArrays.back().get(), true));
		SurfaceRigidBodies.emplace_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
				0, &SurfaceMotionState, SurfaceShapes.back().get(), btVector3(0,0,0))));

		FlagSet<PhysicsCollideClass> CollideType;
		if(surf.Adjoin >= 0) {
			CollideType += PhysicsCollideClass::Adjoin;
		}
		else {
			CollideType += PhysicsCollideClass::Wall;
		}

		FlagSet<PhysicsCollideClass> CollidesWith;
		if(surf.Adjoin < 0 || (surf.Flags & Content::Assets::SurfaceFlag::Impassable) ||
				!(Level.Adjoins[surf.Adjoin].Flags & Content::Assets::SurfaceAdjoinFlag::AllowMovement) ||
				(Level.Adjoins[surf.Adjoin].Flags & Content::Assets::SurfaceAdjoinFlag::AllowAiOnly)) {
			CollidesWith += PhysicsCollideClass::Player;
		}

		DynamicsWorld.addRigidBody(SurfaceRigidBodies.back().get(), static_cast<unsigned int>(CollideType), static_cast<unsigned int>(CollidesWith));

		i += (surf.Vertices.size() - 2) * 3;
	}

	for(const auto& sec : Level.Sectors) {
		for(size_t i = sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
			SurfaceObjectData[i].SurfaceId = i;
			SurfaceObjectData[i].SectorId = sec.Number;
		}
	}

	for(size_t i = 0; i < Level.Surfaces.size(); ++i) {
		SurfaceRigidBodies[i]->setUserPointer(&SurfaceObjectData[i]);
	}

	// HACK: Add camera
	btScalar mass = 1;
	btVector3 fallInertia(0,0,0);
	CameraShape.calculateLocalInertia(mass, fallInertia);
	CameraRigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(
			btRigidBody::btRigidBodyConstructionInfo(mass, &CameraMotionState, &CameraShape, fallInertia)));
	CameraRigidBody->setFlags(BT_DISABLE_WORLD_GRAVITY);
	CameraRigidBody->setSleepingThresholds(0, 0);
	CameraRigidBody->setUserPointer(&CameraObjectData);

	FlagSet<PhysicsCollideClass> CameraCollideType {PhysicsCollideClass::Player};
	FlagSet<PhysicsCollideClass> CameraCollideWith {PhysicsCollideClass::Wall, PhysicsCollideClass::Adjoin};

	DynamicsWorld.addRigidBody(CameraRigidBody.get(), static_cast<unsigned int>(CameraCollideType),
			static_cast<unsigned int>(CameraCollideWith));

	return;
}
