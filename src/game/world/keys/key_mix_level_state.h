#pragma once

#include "content/assets/animation.h"

namespace gorc {
namespace game {
namespace world {
namespace keys {

class key_mix_level_state {
public:
	content::assets::animation const* animation = nullptr;
	double frame;
	int priority;
};

}
}
}
}
