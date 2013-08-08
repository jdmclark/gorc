#pragma once

#include "animation.h"
#include "framework/math/vector.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

namespace Animations {

class SurfaceLightAnimation : public Animation {
private:
	LevelModel& model;
	unsigned int surface;
	float start_light, end_light, change_time, anim_time;

public:
	SurfaceLightAnimation(LevelModel& model, unsigned int surface, float start_light, float end_light, float change_time);

	void Update(double dt);
};

}
}
}
}
}
