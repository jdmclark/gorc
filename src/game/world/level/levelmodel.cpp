#include "levelmodel.h"
#include "game/world/level/physics/physicscollideclass.h"
#include "framework/flagset.h"
#include "content/manager.h"
#include "game/constants.h"

Gorc::Game::World::Level::LevelModel::LevelModel(Gorc::Content::Manager& ContentManager, Cog::Compiler& CogCompiler,
		const Gorc::Content::Assets::Level& Level, const Content::Assets::Inventory& inv)
	: Level(Level), Header(Level.Header), Adjoins(Level.Adjoins), Sectors(Level.Sectors), InventoryModel(inv),
	  Dispatcher(&CollisionConfiguration, *this), DynamicsWorld(&Dispatcher, &Broadphase, &ConstraintSolver, &CollisionConfiguration),
	  SurfaceMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0))), SurfaceObjectData(Level.Surfaces.size()) {
	std::copy(Level.Surfaces.begin(), Level.Surfaces.end(), std::back_inserter(Surfaces));

	DynamicsWorld.setGravity(btVector3(0, 0, -Level.Header.WorldGravity * PhysicsWorldScale));

	// Construct surface rigid bodies.
	for(const auto& sec : Level.Sectors) {
		for(size_t i = sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
			SurfaceObjectData[i].SurfaceId = i;
			SurfaceObjectData[i].SectorId = sec.Number;
		}
	}

	for(size_t i = 0; i < Level.Surfaces.size(); ++i) {
		const auto& surf = Level.Surfaces[i];

		btRigidBody::btRigidBodyConstructionInfo surf_ci(0, &SurfaceMotionState,
				const_cast<btConvexHullShape*>(Level.SurfaceShapes[i].get()),
				btVector3(0,0,0));
		if(!(surf.Flags & Flags::SurfaceFlag::Floor)) {
			surf_ci.m_friction = 0.0f;
		}

		SurfaceRigidBodies.emplace_back(new btRigidBody(surf_ci));
		SurfaceRigidBodies[i]->setUserPointer(&SurfaceObjectData[i]);

		DynamicsWorld.addRigidBody(SurfaceRigidBodies.back().get());
		UpdateSurfacePhysicsProperties(i, true);
	}

	return;
}

void Gorc::Game::World::Level::LevelModel::UpdateSurfacePhysicsProperties(int surface, bool initial) {
	auto& surf = Surfaces[surface];
	if(surf.Adjoin >= 0 && (Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowMovement)) {
		// I guess, apparently, if you set the passable flag for an adjoin, the game
		// automatically clears the impassable flag on the surface?
		surf.Flags -= Flags::SurfaceFlag::Impassable;
	}

	FlagSet<PhysicsCollideClass> CollideType;
	if(surf.Adjoin >= 0) {
		CollideType += PhysicsCollideClass::Adjoin;
	}
	else {
		CollideType += PhysicsCollideClass::Wall;
	}

	if(surf.Flags & Flags::SurfaceFlag::Floor) {
		CollideType += PhysicsCollideClass::Floor;
	}

	FlagSet<PhysicsCollideClass> CollidesWith;
	if(surf.Adjoin < 0 || (surf.Flags & Flags::SurfaceFlag::Impassable) ||
			!(Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowMovement)) {
		CollidesWith += PhysicsCollideClass::Thing;
	}

	if(surf.Adjoin >= 0 && (Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowAiOnly)) {
		CollidesWith += PhysicsCollideClass::Player;
	}

	if(surf.Adjoin >= 0 && (Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowPlayerOnly)) {
		CollidesWith += PhysicsCollideClass::Enemy;
	}

	auto& physics_data = SurfaceObjectData[surface];
	physics_data.CollisionGroup = CollideType;
	physics_data.CollisionMask = CollidesWith;

	if(!initial) {
		// TODO: Just remove relevant manifolds.
		DynamicsWorld.removeRigidBody(SurfaceRigidBodies[surface].get());
		DynamicsWorld.addRigidBody(SurfaceRigidBodies[surface].get());
	}
}
