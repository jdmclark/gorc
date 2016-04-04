#pragma once

#include "libold/base/content/assets/sound.hpp"
#include "libold/content/flags/sound_flag.hpp"
#include "libold/content/flags/sound_subclass_type.hpp"
#include "utility/flag_set.hpp"

namespace gorc {
namespace content {
namespace assets {

class sound_subclass {
public:
    sound_id sound = invalid_id;
    flag_set<flags::sound_flag> flags;
    float min_radius = 1.0f;
    float max_radius = 4.0f;
    float max_volume = 1.0f;
};

}
}
}
