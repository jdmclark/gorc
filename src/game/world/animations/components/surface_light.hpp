#pragma once

#include "libold/base/utility/entity_id.hpp"
#include "libold/content/flags/anim_flag.hpp"
#include "utility/flag_set.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class surface_light {
public:
    #include "surface_light.uid"

    int surface;
    float start_light, end_light, change_time, anim_time;

    surface_light(int surface, float start_light, float end_light, float change_time);
};

}
}
}
}
}
