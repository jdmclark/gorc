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

	Math::Vector<3> sb0, sb1;
	Math::Vector<2> tb0, tb1;

public:
	SlideSurfaceAnimation(LevelModel& model, unsigned int surface, const Math::Vector<3>& direction);

	void Update(double dt);
};

}
}
}
}
}
