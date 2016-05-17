#pragma once

#include "content/id.hpp"
#include "libold/content/flags/anim_flag.hpp"
#include "utility/flag_set.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class surface_light {
public:
    uid(3679965837);

    surface_id surface;
    float start_light, end_light, change_time, anim_time;

    surface_light(surface_id surface, float start_light, float end_light, float change_time);
};

}
}
}
}
}
