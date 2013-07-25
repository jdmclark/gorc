#pragma once

#include "framework/math/box.h"
#include <btBulletDynamicsCommon.h>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

class SectorBroadphaseFilter : public btOverlapFilterCallback {
private:
	const LevelModel& model;
	static const size_t sector_scratch_sz = 16;
	using scratch_array = std::array<unsigned int, sector_scratch_sz>;

	bool computeSectorCover(unsigned int source_sector, unsigned int dest_sector, const Math::Box<3>& bbox,
			scratch_array& sector_scratch, size_t& sector_amt) const;

public:
	SectorBroadphaseFilter(const LevelModel& model);

	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override;
};

}
}
}
}
