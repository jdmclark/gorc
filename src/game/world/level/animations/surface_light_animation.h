#pragma once

#include "animation.h"
#include "framework/math/vector.h"

namespace gorc {
namespace game {
namespace world {
namespace level {

class level_model;

namespace animations {

class surface_light_animation : public animation {
private:
	level_model& model;
	unsigned int surface;
	float start_light, end_light, change_time, anim_time;

public:
	surface_light_animation(level_model& model, unsigned int surface, float start_light, float end_light, float change_time, int anim_num);

	void update(double dt);
	void stop();
};

}
}
}
}
}
