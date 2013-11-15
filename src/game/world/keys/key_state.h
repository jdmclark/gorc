#pragma once

#include "content/assets/animation.h"

namespace gorc {
namespace game {
namespace world {
namespace keys {

class key_mix;

class key_state {
public:
	int mix_id;
	content::assets::animation const* animation = nullptr;
	double animation_time;
	double current_frame;
	double speed;
	flag_set<flags::key_flag> flags;
	float expiration_time = 0.0f;

	int high_priority, low_priority, body_priority;
};

}
}
}
}
