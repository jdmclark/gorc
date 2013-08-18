#include "sectorcollisiondispatcher.h"
#include "game/world/level/levelmodel.h"
#include "physicsobjectdata.h"
#include "framework/math/box.h"
#include "game/constants.h"
#include <iostream>

bool Gorc::Game::World::Level::SectorCollisionDispatcher::ComputeSectorCover(unsigned int source_sector, unsigned int dest_sector,
		const Math::Box<3>& bbox, SectorScratchArray<>& sector_scratch) const {
	if(source_sector == dest_sector) {
		return true;
	}

	// Test sector against bbox.
	if(!Math::BoxesOverlap(model.Sectors[source_sector].BoundingBox, bbox)) {
		return false;
	}

	// Look for sector in scratch list. If in list, already handled; return false.
	if(std::find(sector_scratch.begin(), sector_scratch.end(), source_sector) != sector_scratch.end()) {
		return false;
	}

	// Try to insert sector.
	// If not enough room, object or motion is very large - return true immediately.
	if(sector_scratch.full()) {
		return true;
	}
	else {
		sector_scratch.push_back(source_sector);
	}

	const Content::Assets::LevelSector& sec = model.Sectors[source_sector];
	for(int i = 0; i < sec.SurfaceCount; ++i) {
		const Content::Assets::LevelSurface& surf = model.Surfaces[sec.FirstSurface + i];
		if(surf.Adjoin >= 0) {
			if(ComputeSectorCover(surf.AdjoinedSector, dest_sector, bbox, sector_scratch)) {
				return true;
			}
		}
	}

	return false;
}

Gorc::Math::Box<3> Gorc::Game::World::Level::SectorCollisionDispatcher::GetTemporalAabb(btRigidBody* body) {
	btVector3 aabb_min, aabb_max;

	btTransform worldTrans;
	body->getMotionState()->getWorldTransform(worldTrans);

	body->getCollisionShape()->calculateTemporalAabb(worldTrans, body->getLinearVelocity(), body->getAngularVelocity(),
			GameplayTick, aabb_min, aabb_max);
	const Math::Vector<3> margin = Math::Zero<3>(0.1f);
	return Math::Box<3>(Math::VecBt(aabb_min) * PhysicsInvWorldScale - margin, Math::VecBt(aabb_max) * PhysicsInvWorldScale + margin);
}

Gorc::Game::World::Level::SectorCollisionDispatcher::SectorCollisionDispatcher(btCollisionConfiguration* collisionConfiguration,
		const LevelModel& model)
	: btCollisionDispatcher(collisionConfiguration), model(model) {
	return;
}

bool Gorc::Game::World::Level::SectorCollisionDispatcher::needsCollision(btCollisionObject* body0, btCollisionObject* body1) {
	PhysicsObjectData* userData0 = reinterpret_cast<PhysicsObjectData*>(body0->getUserPointer());
	Math::Box<3> box0 = GetTemporalAabb(reinterpret_cast<btRigidBody*>(body0));

	PhysicsObjectData* userData1 = reinterpret_cast<PhysicsObjectData*>(body1->getUserPointer());
	Math::Box<3> box1 = GetTemporalAabb(reinterpret_cast<btRigidBody*>(body1));

	SectorScratchArray<> scratch0, scratch1;
	if(ComputeSectorCover(userData0->SectorId, userData1->SectorId, box0, scratch0)
			|| ComputeSectorCover(userData1->SectorId, userData0->SectorId, box1, scratch1)) {
		return btCollisionDispatcher::needsCollision(body0, body1);
	}

	// Look for an overlap
	if(!IsSectorScratchDisjoint(scratch0, scratch1)) {
		return btCollisionDispatcher::needsCollision(body0, body1);
	}

	return false;
}

bool Gorc::Game::World::Level::SectorCollisionDispatcher::needsResponse(btCollisionObject* body0, btCollisionObject* body1) {
	PhysicsObjectData* userData0 = reinterpret_cast<PhysicsObjectData*>(body0->getUserPointer());
	PhysicsObjectData* userData1 = reinterpret_cast<PhysicsObjectData*>(body1->getUserPointer());

	bool maskCollides = (userData0->CollisionGroup & userData1->CollisionMask) != 0;
	maskCollides = maskCollides && (userData1->CollisionGroup & userData0->CollisionMask) != 0;
	if(!maskCollides) {
		return false;
	}

	Math::Box<3> box0 = GetTemporalAabb(reinterpret_cast<btRigidBody*>(body0));
	Math::Box<3> box1 = GetTemporalAabb(reinterpret_cast<btRigidBody*>(body1));

	SectorScratchArray<> scratch0, scratch1;
	if(ComputeSectorCover(userData0->SectorId, userData1->SectorId, box0, scratch0)
			|| ComputeSectorCover(userData1->SectorId, userData0->SectorId, box1, scratch1)) {
		return btCollisionDispatcher::needsResponse(body0, body1);
	}

	// Look for an overlap
	if(!IsSectorScratchDisjoint(scratch0, scratch1)) {
		return btCollisionDispatcher::needsResponse(body0, body1);
	}

	return false;
}
