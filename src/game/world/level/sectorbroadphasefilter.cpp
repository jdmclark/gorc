#include "sectorbroadphasefilter.h"
#include "levelmodel.h"
#include "physicsobjectdata.h"
#include "framework/math/box.h"

Gorc::Game::World::Level::SectorBroadphaseFilter::SectorBroadphaseFilter(const LevelModel& model)
	: model(model) {
	return;
}

bool Gorc::Game::World::Level::SectorBroadphaseFilter::computeSectorCover(unsigned int source_sector, unsigned int dest_sector,
		const Math::Box<3>& bbox, std::array<unsigned int, sector_scratch_sz>& sector_scratch, size_t& sector_count) const {
	if(source_sector == dest_sector) {
		return true;
	}

	// Test sector against bbox.
	if(!Math::BoxesOverlap(model.Sectors[source_sector].BoundingBox, bbox)) {
		return false;
	}

	// Look for sector in scratch list. If in list, already handled; return false.
	if(std::find(sector_scratch.begin(), sector_scratch.begin() + sector_count, source_sector) != sector_scratch.begin() + sector_count) {
		return false;
	}

	// Try to insert sector.
	// If not enough room, object or motion is very large - return true immediately.
	if(sector_count < sector_scratch_sz) {
		sector_scratch[sector_count++] = source_sector;
	}
	else {
		return true;
	}

	const Content::Assets::LevelSector& sec = model.Sectors[source_sector];
	for(int i = 0; i < sec.SurfaceCount; ++i) {
		const Content::Assets::LevelSurface& surf = model.Surfaces[sec.FirstSurface + i];
		if(surf.Adjoin >= 0) {
			if(computeSectorCover(surf.AdjoinedSector, dest_sector, bbox, sector_scratch, sector_count)) {
				return true;
			}
		}
	}

	return false;
}

bool Gorc::Game::World::Level::SectorBroadphaseFilter::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const {
	bool maskCollides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
	maskCollides = maskCollides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask) != 0;
	if(!maskCollides) {
		return false;
	}

	btCollisionObject* object0 = reinterpret_cast<btCollisionObject*>(proxy0->m_clientObject);
	PhysicsObjectData* userData0 = reinterpret_cast<PhysicsObjectData*>(object0->getUserPointer());
	Math::Box<3> box0 = Math::Box<3>(Math::VecBt(proxy0->m_aabbMin), Math::VecBt(proxy0->m_aabbMax));

	btCollisionObject* object1 = reinterpret_cast<btCollisionObject*>(proxy1->m_clientObject);
	PhysicsObjectData* userData1 = reinterpret_cast<PhysicsObjectData*>(object1->getUserPointer());
	Math::Box<3> box1 = Math::Box<3>(Math::VecBt(proxy1->m_aabbMin), Math::VecBt(proxy1->m_aabbMax));

	std::array<unsigned int, sector_scratch_sz> scratch0, scratch1;
	size_t sector_amt0 = 0, sector_amt1 = 0;
	if(computeSectorCover(userData0->SectorId, userData1->SectorId, box0, scratch0, sector_amt0)
			|| computeSectorCover(userData1->SectorId, userData0->SectorId, box1, scratch1, sector_amt1)) {
		return true;
	}

	// Look for an overlap
	for(int i = 0; i < sector_amt0; ++i) {
		for(int j = 0; j < sector_amt1; ++j) {
			if(scratch0[i] == scratch1[j]) {
				return true;
			}
		}
	}

	return false;
}
