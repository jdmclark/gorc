#pragma once

#include "content/assets/level_surface.h"
#include "animations/animation.h"
#include "framework/pool.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Surface : public Content::Assets::LevelSurface {
public:
	int CelNumber = -1;
	Id<PoolPtr<Animations::Animation>> AnimNumber;

	Surface(const Content::Assets::LevelSurface& surf);
	const Content::Assets::LevelSurface& operator=(const Content::Assets::LevelSurface& surf);
};

}
}
}
}
