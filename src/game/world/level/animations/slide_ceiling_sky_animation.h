#pragma once

#include "animation.h"
#include "framework/math/vector.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

namespace Animations {

class SlideCeilingSkyAnimation : public Animation {
private:
	LevelModel& model;
	Math::Vector<2> speed;

public:
	SlideCeilingSkyAnimation(LevelModel& model, const Math::Vector<2>& speed);

	void Update(double dt);
};

}
}
}
}
}
