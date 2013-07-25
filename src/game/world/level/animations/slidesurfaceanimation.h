#pragma once

#include "animation.h"
#include "framework/math/vector.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

namespace Animations {

class SlideSurfaceAnimation : public Animation {
private:
	LevelModel& model;
	unsigned int surface;
	Math::Vector<3> direction;

public:
	SlideSurfaceAnimation(LevelModel& model, unsigned int surface, const Math::Vector<3>& direction);

	void Update(double dt);
};

}
}
}
}
}
