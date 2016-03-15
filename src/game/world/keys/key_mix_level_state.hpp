#pragma once

#include "libold/content/assets/animation.hpp"

namespace gorc {
namespace game {
namespace world {
namespace keys {

class key_mix_level_state {
public:
    maybe<content::assets::animation const*> prev_animation;
    double prev_frame = 0.0;
    double prev_frame_blend = 0.0;

    maybe<content::assets::animation const*> animation;
    double frame;
    int priority;
    unsigned int key_timestamp;
};

}
}
}
}
