#pragma once

#include "content/assets/level_surface.h"
#include "animations/animation.h"
#include "framework/pool.h"
#include "physics/objectdata.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Surface : public Content::Assets::LevelSurface {
public:
	Physics::SurfaceObjectData ObjectData;

	int CelNumber = -1;
	int AnimNumber = -1;

	inline const Physics::SurfaceObjectData& GetObjectData() const {
		return ObjectData;
	}

	Surface(const Content::Assets::LevelSurface& surf);
};

}
}
}
}
