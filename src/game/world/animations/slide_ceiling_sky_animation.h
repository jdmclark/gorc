#pragma once

#include "animation.h"
#include "framework/math/vector.h"

namespace gorc {
namespace game {
namespace world {

class level_model;

namespace animations {

class slide_ceiling_sky_animation : public animation {
private:
	level_model& model;
	vector<2> speed;

public:
	slide_ceiling_sky_animation(level_model& model, const vector<2>& speed);

	void update(double dt);
};

}
}
}
}
