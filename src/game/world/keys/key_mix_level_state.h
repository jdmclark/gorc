#pragma once

#include "content/assets/animation.h"

namespace gorc {
namespace game {
namespace world {
namespace keys {

class key_mix_level_state {
public:
	content::assets::animation const* prev_animation = nullptr;
	double prev_frame = 0.0;
	double prev_frame_blend = 0.0;

	content::assets::animation const* animation = nullptr;
	double frame;
	int priority;
	unsigned int key_timestamp;
};

}
}
}
}
