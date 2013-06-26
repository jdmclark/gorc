#include "levelmodel.h"
#include "framework/flagset.h"

Gorc::Game::World::Level::PhysicsObjectData::~PhysicsObjectData() {
	return;
}

Gorc::Game::World::Level::Thing::Thing(const Content::Assets::Template& tpl)
	: Content::Assets::Template(tpl) {
	return;
}

Gorc::Game::World::Level::LevelModel::LevelModel(const Gorc::Content::Assets::Level& Level)
	: Level(Level), Dispatcher(&CollisionConfiguration), DynamicsWorld(&Dispatcher, &Broadphase, &ConstraintSolver, &CollisionConfiguration),
	  SurfaceMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0))), SurfaceObjectData(Level.Surfaces.size()) {
	DynamicsWorld.setGravity(btVector3(0, 0, -Level.Header.WorldGravity));

	size_t i = 0;
	for(const auto& surf : Level.Surfaces) {
		SurfaceRigidBodies.emplace_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
				0, &SurfaceMotionState, const_cast<btConvexHullShape*>(&surf.SurfaceShape), btVector3(0,0,0))));

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

	// HACK: Enumerate spawn points
	for(const auto& thing : Level.Things) {
		if(thing.Type == Content::Assets::ThingType::Player) {
			SpawnPoints.push_back(&thing);
		}
	}

	// HACK: Add camera
	CurrentSpawnPoint = 0;
	CameraCurrentSector = SpawnPoints[CurrentSpawnPoint]->Sector;
	CameraPosition = SpawnPoints[CurrentSpawnPoint]->Position;

	CameraShape = std::unique_ptr<btSphereShape>(new btSphereShape(CameraRadius));
	CameraMotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), BtVec(CameraPosition))));

	btScalar mass = SpawnPoints[CurrentSpawnPoint]->Mass;
	btVector3 fallInertia(0,0,0);
	CameraShape->calculateLocalInertia(mass, fallInertia);
	CameraRigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(
			btRigidBody::btRigidBodyConstructionInfo(mass, CameraMotionState.get(), CameraShape.get(), fallInertia)));
	CameraRigidBody->setFlags(BT_DISABLE_WORLD_GRAVITY);
	CameraRigidBody->setSleepingThresholds(0, 0);
	CameraRigidBody->setUserPointer(&CameraObjectData);

	FlagSet<PhysicsCollideClass> CameraCollideType {PhysicsCollideClass::Player};
	FlagSet<PhysicsCollideClass> CameraCollideWith {PhysicsCollideClass::Wall, PhysicsCollideClass::Adjoin, PhysicsCollideClass::Player};

	DynamicsWorld.addRigidBody(CameraRigidBody.get(), static_cast<unsigned int>(CameraCollideType),
			static_cast<unsigned int>(CameraCollideWith));

	// HACK: Create thing collision shapes and rigid bodies
	for(const auto& thing : Level.Things) {
		if(thing.Type != Content::Assets::ThingType::Player && thing.Type != Content::Assets::ThingType::Actor && thing.Model3d) {
			Things.emplace_back(thing);
			static const float deg2rad = 0.0174532925f;
			btQuaternion orientation = btQuaternion(btVector3(1.0f, 0.0f, 0.0f), deg2rad * Math::Get<0>(thing.Orientation))
					* btQuaternion(btVector3(0.0f, 0.0f, 1.0f), deg2rad * Math::Get<1>(thing.Orientation))
					* btQuaternion(btVector3(0.0f, 1.0f, 0.0f), deg2rad * Math::Get<2>(thing.Orientation));

			float thing_mass = (Things.back().Move == Content::Assets::MoveType::Physics) ? Things.back().Mass : 0.0f;
			btVector3 thing_inertia(0,0,0);
			Things.back().Model3d->Shape->calculateLocalInertia(thing_mass, thing_inertia);

			Things.back().MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(
					btTransform(orientation, Math::BtVec(thing.Position))));
			Things.back().RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(
					btRigidBody::btRigidBodyConstructionInfo(thing_mass, Things.back().MotionState.get(),
							Things.back().Model3d->Shape.get(), thing_inertia)));
			DynamicsWorld.addRigidBody(Things.back().RigidBody.get(), static_cast<unsigned int>(CameraCollideType),
					static_cast<unsigned int>(CameraCollideWith));
		}
	}

	return;
}
