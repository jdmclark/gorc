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

	size_t i = 0;
	for(const auto& surf : Level.Surfaces) {
		SurfaceRigidBodies.emplace_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
				0, &SurfaceMotionState, surf.SurfaceShape.get(), btVector3(0,0,0))));

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
