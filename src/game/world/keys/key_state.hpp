#pragma once

#include "libold/content/assets/animation.hpp"

namespace gorc {
namespace game {
namespace world {
namespace keys {

class key_mix;

class key_state {
public:
    int mix_id;
    maybe<asset_ref<content::assets::animation>> animation;
    double animation_time;
    double current_frame;
    double speed;
    flag_set<flags::key_flag> flags;
    float expiration_time = 0.0f;
    unsigned int creation_timestamp = 0;

    int high_priority, low_priority, body_priority;
};

}
}
}
}
