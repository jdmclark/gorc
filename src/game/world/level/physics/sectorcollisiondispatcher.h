#pragma once

#include "framework/math/box.h"
#include <btBulletDynamicsCommon.h>
#include <array>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

template <size_t sector_scratch_sz = 16> class SectorScratchArray {
private:
	std::array<unsigned int, sector_scratch_sz> scratch;
	size_t top = 0;

public:
	inline auto begin() -> decltype(scratch.begin()) {
		return scratch.begin();
	}

	inline auto begin() const -> decltype(scratch.begin()) {
		return scratch.begin();
	}

	inline auto end() -> decltype(scratch.begin()) {
		return scratch.begin() + top;
	}

	inline auto end() const -> decltype(scratch.begin()) {
		return scratch.begin() + top;
	}

	bool full() const {
		return top == sector_scratch_sz;
	}

	void push_back(unsigned int i) {
		scratch[top] = i;
		++top;
	}
};

template <size_t ssz_a, size_t ssz_b> bool IsSectorScratchDisjoint(const SectorScratchArray<ssz_a>& a,
		const SectorScratchArray<ssz_b>& b) {
	for(auto a_v : a) {
		for(auto b_v : b) {
			if(a_v == b_v) {
				return false;
			}
		}
	}

	return true;
}

class SectorCollisionDispatcher : public btCollisionDispatcher {
private:
	const LevelModel& model;

	bool ComputeSectorCover(unsigned int source_sector, unsigned int dest_sector, const Math::Box<3>& bbox,
				SectorScratchArray<>& sector_scratch) const;
	Math::Box<3> GetTemporalAabb(btRigidBody* body);

public:
	SectorCollisionDispatcher(btCollisionConfiguration* collisionConfiguration, const LevelModel& model);

	virtual bool needsCollision(btCollisionObject* body0, btCollisionObject* body1) override;
	virtual bool needsResponse(btCollisionObject* body0, btCollisionObject* body1) override;
};

}
}
}
}
