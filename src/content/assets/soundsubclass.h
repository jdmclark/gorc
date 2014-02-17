#pragma once

#include "framework/content/assets/sound.h"
#include "content/flags/sound_flag.h"
#include "content/flags/sound_subclass_type.h"
#include "framework/utility/flag_set.h"

namespace gorc {
namespace content {
namespace assets {

class sound_subclass {
public:
    int sound = -1;
    flag_set<flags::sound_flag> flags;
    float min_radius = 1.0f;
    float max_radius = 4.0f;
    float max_volume = 1.0f;
};

}
}
}
